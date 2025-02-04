/*
 * Copyright (c) 2021, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <ctime>
#include <mutex>

#include <Lib/Logging.h>
#include <Lib/Options.h>

namespace Lib {

std::mutex g_logging_mutex;

char const *LogLevel_name(LogLevel level)
{
    switch (level) {
#undef S
#define S(level, cardinal) \
    case LogLevel::level:  \
        return #level;
        ENUMERATE_LOG_LEVELS(S)
#undef S
    default:
        fatal("Unreachable {}", "LogLevel_name");
    }
}

std::optional<LogLevel> LogLevel_by_name(std::string_view const &name)
{
#undef S
#define S(level, cardinal)      \
    if (name == #level) {       \
        return LogLevel::level; \
    }
    ENUMERATE_LOG_LEVELS(S)
#undef S
    return {};
}

LogCategory::LogCategory(std::string_view name) noexcept
    : name(name)
{
}

std::clock_t LogCategory::start()
{
    return std::clock();
}

Logger::Logger()
{
    auto add_category = [this](std::string_view const &category) {
        if (category == "all") {
            m_all_enabled = true;
        } else {
            m_categories.insert(category);
        }
    };
    auto add_categories = [add_category](std::string_view const &categories) {
        auto prev = 0ul;
        for (auto sep = categories.find_first_of(";,:", prev); sep != std::string_view::npos; prev = sep, sep = categories.find_first_of(";,:", prev + 1)) {
            add_category(categories.substr(prev, sep - prev));
        }
        if (prev < categories.length()) {
            add_category(categories.substr(prev));
        }
    };
    auto set_level = [this](std::string_view level) {
        auto lvl = LogLevel_by_name(level);
        if (lvl.has_value()) {
            m_level = lvl.value();
        }
    };
    if (auto logfile = get_option("logfile"); logfile.has_value()) {
        m_logfile = logfile.value();
    } else if (auto env_logfile = getenv("EDDY_LOGFILE"); env_logfile) {
        m_logfile = env_logfile;
    }
    if (auto loglevel = get_option("loglevel"); loglevel.has_value()) {
        set_level(loglevel.value());
    } else if (auto env_level = getenv("EDDY_LOGLEVEL"); env_level && *env_level) {
        set_level(env_level);
    }
    auto categories = get_option_values("trace");
    for (auto cat : categories) {
        add_categories(cat);
    }
    if (auto env_trace = getenv("EDDY_TRACE"); env_trace != nullptr) {
        std::string_view trace = env_trace;
        auto             prev = 0ul;
        for (auto sep = trace.find_first_of(";,:", prev); sep != std::string_view::npos; prev = sep, sep = trace.find_first_of(";,:", prev + 1)) {
            add_categories(trace.substr(prev, sep - prev));
        }
        add_categories(trace.substr(prev));
    }
}

Logger &Logger::get_logger()
{
    static Logger                    *logger = nullptr;
    std::lock_guard<std::mutex> const lock(g_logging_mutex);
    if (!logger)
        logger = new Logger();
    return *logger;
}

}
