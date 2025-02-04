/*
 * Copyright (c) 2021, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cassert>
#include <cstdlib>
#include <ctime>
#include <format>
#include <iostream>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <sys/signal.h>

namespace Lib {

// Want this somewhere that's included basically everywhere.
template<class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};

#define ENUMERATE_LOG_LEVELS(S) \
    S(None, 0)                  \
    S(Trace, 1)                 \
    S(Info, 2)                  \
    S(Warning, 3)               \
    S(Error, 4)                 \
    S(Fatal, 5)

enum class LogLevel {
#undef S
#define S(level, cardinal) level = cardinal,
    ENUMERATE_LOG_LEVELS(S)
#undef S
};

char const             *LogLevel_name(LogLevel);
std::optional<LogLevel> LogLevel_by_name(std::string_view const &);

class Logger;
class LogCategory;

struct LogMessage {
    std::string_view file;
    size_t           line;
    std::string_view function;
    std::string_view category;
    LogLevel         level;
    std::string      message;
};

extern std::mutex g_logging_mutex;

class Logger {
public:
    template<typename... Args>
    constexpr void logmsg(LogMessage const &msg, Args const &...args)
    {
        std::lock_guard<std::mutex> const lock(g_logging_mutex);
        if (!msg.category.empty() && !m_categories.contains(msg.category) && !m_all_enabled) {
            return;
        }
        if (msg.level >= m_level) {
            std::string_view f(msg.file);
            if (f.front() == '/') {
                auto ix = f.find_last_of('/');
                if (ix != std::string_view::npos) {
                    auto len = f.length() - ix - 1;
                    if (len > 19) {
                        len = 19;
                    }
                    f = f.substr(ix + 1, len);
                }
            }
            auto file_line = std::format("{}:{}", f, msg.line);
            auto prefix = std::format("{:<24}:{:<20}:{:<5}:", file_line, msg.function, LogLevel_name(msg.level));
            auto message = std::vformat(msg.message, std::make_format_args(args...));
            std::cerr << prefix << message << std::endl;
            std::cerr.flush();
        }
    }

    template<typename... Args>
    void error_msg(std::string_view const &file, size_t line, std::string_view const &function, char const *message, Args &&...args)
    {
        logmsg({ file, line, function, "", LogLevel::Error, message }, std::forward<Args>(args)...);
        exit(1);
    }

    template<typename... Args>
    __attribute__((noreturn)) void fatal_msg(std::string_view const &file, size_t line, std::string_view const &function, char const *message, Args const &...args)
    {
        logmsg({ file, line, function, "", LogLevel::Fatal, message }, std::forward<Args const &>(args)...);
        abort();
    }

    template<typename... Args>
    void assert_msg(std::string_view const &file, size_t line, std::string_view const &function, bool condition, char const *message, Args const &...args)
    {
        if (condition)
            return;
        logmsg({ file, line, function, "", LogLevel::Fatal, message }, std::forward<Args const &>(args)...);
        abort();
    }
    static Logger &get_logger();

private:
    Logger();

    std::set<std::string_view> m_categories {};
    LogLevel                   m_level { LogLevel::Trace };
    std::string                m_logfile {};
    bool                       m_all_enabled { false };
};

struct LogCategory {
public:
    std::string name {};

    LogCategory(std::string_view name) noexcept;
    LogCategory(LogCategory const &) = default;

    template<typename... Args>
    void logmsg(LogMessage const &msg, Args &&...args)
    {
        Logger::get_logger().logmsg(msg, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void trace_msg(std::string_view const &file, size_t line, std::string_view const &function, char const *message, Args &&...args)
    {
        Logger::get_logger().logmsg({ file, line, function, name, LogLevel::Trace, message }, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info_msg(std::string_view const &file, size_t line, std::string_view const &function, char const *message, Args &&...args)
    {
        Logger::get_logger().logmsg({ file, line, function, name, LogLevel::Info, message }, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warning_msg(std::string_view const &file, size_t line, std::string_view const &function, char const *message, Args &&...args)
    {
        Logger::get_logger().logmsg({ file, line, function, name, LogLevel::Warning, message }, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error_msg(std::string_view const &file, size_t line, std::string_view const &function, char const *message, Args &&...args)
    {
        Logger::get_logger().logmsg({ file, line, function, name, LogLevel::Error, message }, std::forward<Args>(args)...);
    }

    static std::clock_t start();

    template<typename... Args>
    void log_duration(std::clock_t clock_start, std::string_view const &file, size_t line, std::string_view const &caller, char const *msg, Args &&...args)
    {
        auto clock_end = std::clock();
        auto duration_ms = (unsigned long) (1000.0 * ((float) clock_end - (float) clock_start) / CLOCKS_PER_SEC);

        auto       msg_with_timing = std::string(msg).append(" {d}.{03d} sec");
        LogMessage log_message {
            file,
            line,
            caller,
            name,
            LogLevel::Trace,
            msg_with_timing,
        };
        logmsg(log_message, std::forward<Args>(args)..., duration_ms / 1000, duration_ms % 1000);
    }
};

#define trace(module, fmt, args...) Lib::LogCategory(#module).trace_msg(__FILE__, __LINE__, __func__, fmt, ##args)
#define info(module, fmt, args...) Lib::LogCategory(#module).info_msg(__FILE__, __LINE__, __func__, fmt, ##args)
#define warning(module, fmt, args...) Lib::LogCategory(#module).warning_msg(__FILE__, __LINE__, __func__, fmt, ##args)
#define log_timestamp_start(module) (Lib::LogCategory(#module).start())
#define log_timestamp_end(module, ts, fmt, ...) Lib::LogCategory(#module).log_duration(ts, __FILE__, __LINE__, __func__, fmt, ##args)

#define log_error(fmt, args...) Lib::Logger::get_logger().error_msg(__FILE__, __LINE__, __func__, fmt, ##args)
#define fatal(fmt, args...) Lib::Logger::get_logger().fatal_msg( \
    __FILE__, __LINE__, __func__,                                \
    fmt, ##args)
#ifdef assert
#undef assert
#endif
#define arwen_assert(condition) Lib::Logger::get_logger().assert_msg(__FILE__, __LINE__, __func__, condition, "Assertion error: {}", #condition)
#define assert(condition) arwen_assert(condition)
#define assert_with_msg(condition, fmt, args...) Lib::Logger::get_logger().assert_msg(__FILE__, __LINE__, __func__, condition, "Assertion error: " #condition ": " fmt, ##args)

#define UNREACHABLE() fatal("Unreachable")
#define NYI() fatal("Not yet implemented")

}
