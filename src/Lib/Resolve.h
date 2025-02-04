/*
 * Copyright (c) 2021, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <algorithm>
#include <filesystem>
#include <format>
#include <functional>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include <Lib/Lib.h>
#include <Lib/Logging.h>
#include <Lib/Result.h>
#include <Lib/SimpleFormat.h>

namespace Lib {

namespace fs = std::filesystem;

constexpr static char const *ARWEN_DIR = "ARWEN_DIR";
constexpr static char const *ARWEN_INIT = "_arwen_init";

using void_t = void (*)();
using lib_handle_t = void *;

#define ResolveErrorCodes(S) \
    S(CouldNotFindLibrary)   \
    S(DLError)               \
    S(InvalidFunctionReference)

enum ResolveErrorCode {
#undef S
#define S(E) E,
    ResolveErrorCodes(S)
#undef S
};

template<>
inline std::string_view to_string(ResolveErrorCode const &e)
{
    switch (e) {
#undef S
#define S(E) \
    case E:  \
        return #E;
        ResolveErrorCodes(S)
#undef S
            default : UNREACHABLE();
    }
}

}

template<>
struct std::formatter<Lib::ResolveErrorCode, char> : public Lib::SimpleFormatParser {
    template<class FmtContext>
    typename FmtContext::iterator format(Lib::ResolveErrorCode const &e, FmtContext &ctx) const
    {
        std::ostringstream out;
        out << to_string(e);
        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};

namespace Lib {

struct ResolveError {
    ResolveErrorCode code;
    std::string      message;

    ResolveError(ResolveErrorCode c, std::string_view m)
        : code(c)
    {
        message = std::format("{}: {}", code, m);
    }
};

template<>
inline std::string_view to_string(ResolveError const &e)
{
    return e.message;
}

using LibOpenResult = Result<lib_handle_t, ResolveError>;
using ResolveResult = Result<void_t, ResolveError>;

struct FunctionName {
    std::string library;
    std::string function;

    static Result<FunctionName, ResolveError> make(std::string_view uri)
    {
        if (auto paren = uri.find_first_of('('); paren != std::string::npos) {
            uri = uri.substr(0, paren);
        }
        uri = trim(uri);
        std::string_view lib_name;
        std::string_view function;
        if (auto colon = uri.find(':'); colon != std::string::npos) {
            if (uri.find(':', colon + 1) != std::string::npos) {
                return ResolveError { ResolveErrorCode::InvalidFunctionReference, std::format("Invalid function reference '{}'", uri) };
            }
            lib_name = uri.substr(0, colon);
            function = uri.substr(colon + 1);
        } else {
            function = uri;
        }
        return FunctionName { std::string(lib_name), std::string(function) };
    }
};

class Resolver {
public:
    ~Resolver() = default;
    static Resolver &get_resolver() noexcept;
    LibOpenResult    open(std::string_view const &);

    template<typename FncType>
    Result<std::function<FncType>, ResolveError> resolve(std::string_view const &func_name)
    {
        return resolve<FncType>(TRY_EVAL(FunctionName::make(func_name)));
    }

    template<typename FncType>
    Result<std::function<FncType>, ResolveError> resolve(FunctionName const &func_name)
    {
        return std::function<FncType> { (FncType *) (TRY_EVAL(resolve_(func_name))) };
    }

private:
    class Library {
    public:
        explicit Library(std::string_view img);

        ~Library();
        std::string                to_string();
        static fs::path            platform_image(std::string_view const &);
        [[nodiscard]] bool         is_valid() const { return !m_my_error; }
        [[nodiscard]] lib_handle_t handle() const { return m_handle; }
        [[nodiscard]] ResolveError error() const
        {
            assert(m_my_error.has_value());
            return *m_my_error;
        }
        ResolveResult get_function(std::string const &);

    private:
        LibOpenResult open();
        LibOpenResult search_and_open();
        LibOpenResult try_open(fs::path const &);

        lib_handle_t                            m_handle { nullptr };
        std::string                             m_image {};
        std::optional<ResolveError>             m_my_error {};
        std::unordered_map<std::string, void_t> m_functions {};
        friend Resolver;
    };

    Resolver() = default;
    ResolveResult                            resolve_(FunctionName const &);
    std::unordered_map<std::string, Library> m_images;
};

// TODO: Bring search path shenanigans into here from Resolver.
struct SearchingResolver {
    std::string prefix {};
    std::string lib {};

    SearchingResolver() = default;
    ~SearchingResolver() = default;

    static LibOpenResult open(std::string_view const &lib_name)
    {
        return Resolver::get_resolver().open(lib_name);
    }

    template<typename FncType>
    Result<std::function<FncType>, ResolveError> resolve(std::string_view const &func_name) const
    {
        return resolve<FncType>(TRY_EVAL(FunctionName::make(func_name)));
    }

    template<typename FncType>
    Result<std::function<FncType>, ResolveError> resolve(FunctionName func_name) const
    {
        auto &resolver = Resolver::get_resolver();
        auto  res = resolver.resolve<FncType>(func_name);
        if (res.has_value() && res.value() != nullptr) {
            return res.value();
        }
        if (!prefix.empty()) {
            std::string fnc_name { prefix + func_name.function };
            func_name.function = fnc_name;
            res = resolver.resolve<FncType>(func_name);
            if (res.has_value() && res.value() != nullptr) {
                return res.value();
            }
        }
        if (!lib.empty() && func_name.library.empty()) {
            func_name.library = lib;
            return resolve<FncType>(func_name);
        }
        return res;
    }
};

}

template<>
struct std::formatter<Lib::ResolveError, char> : Lib::SimpleFormatParser {
    template<class FmtContext>
    typename FmtContext::iterator format(Lib::ResolveError const &e, FmtContext &ctx) const
    {
        std::ostringstream out;
        out << to_string(e);
        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};
