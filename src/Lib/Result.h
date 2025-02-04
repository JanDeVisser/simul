/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <print>
#include <type_traits>
#include <variant>

#include <Lib/Error.h>
#include <Lib/Logging.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"

namespace Lib {

template<typename ResultType, typename ErrorType = LibCError>
class [[nodiscard]] Result {
public:
    using R = Result;
    Result()
        : m_value(ResultType {})
    {
    }

    Result(ResultType const &return_value)
        : m_value(return_value)
    {
    }

    Result(ResultType &&return_value)
        : m_value(std::move(return_value))
    {
    }

    template<typename U>
    Result(U &&value)
        requires(!std::is_same_v<std::remove_cvref_t<U>, ResultType> && !std::is_same_v<std::remove_cvref_t<U>, Result> && !std::is_same_v<std::remove_cvref_t<U>, ErrorType>)
        : m_value(std::forward<U>(value))
    {
    }

    Result(ErrorType const &error)
        : m_value(error)
    {
    }

    Result(ErrorType &&error)
        : m_value(std::move(error))
    {
    }

    Result(Result const &other)
    {
        if (other.is_error()) {
            m_value = other.error();
        } else {
            m_value = other.value();
        }
    }

    ~Result() = default;

    Result &operator=(Result &&other) noexcept
    {
        if (other.is_error()) {
            m_value = std::move(other.error());
        } else {
            m_value = std::move(other.value());
        }
        return *this;
    }

    Result &operator=(Result const &other)
    {
        if (other.is_error()) {
            m_value = other.error();
        } else {
            m_value = other.value();
        }
        return *this;
    }

    [[nodiscard]] bool          has_value() const { return std::holds_alternative<ResultType>(m_value); }
    ResultType const           &value() const { return std::get<ResultType>(m_value); }
    ResultType                 &value() { return std::get<ResultType>(m_value); }
    [[nodiscard]] bool          is_error() const { return std::holds_alternative<ErrorType>(m_value); }
    ErrorType const            &error() const { return std::get<ErrorType>(m_value); }
    ResultType const           &operator*() const noexcept { return value(); }
    ResultType                 &operator*() noexcept { return value(); }
    constexpr ResultType const *operator->() const noexcept { return &value(); }
    constexpr ResultType       *operator->() noexcept { return &value(); }
    explicit                    operator bool() const { return !is_error(); }

    template<typename NewError, typename Adapter>
    Result<ResultType, NewError> adapt(Adapter const &adapter)
    {
        return std::visit(
            overload {
                [adapter](ErrorType &&value) -> Result<ResultType, NewError> {
                    return Result<ResultType, NewError>(std::move(adapter(value)));
                },
                [](ResultType &&value) -> Result<ResultType, NewError> {
                    return Result(std::move(value));
                } },
            m_value);
    }

    template<typename Catch>
    ResultType const &must(Catch const &catch_)
    {
        std::visit(
            overload {
                [this, &catch_](ErrorType const &value) {
                    if (R caught = catch_(value); caught.is_error()) {
                        std::println("Aborting: Result::must() failed");
                        exit(1);
                    } else {
                        *this = std::move(caught);
                    }
                },
                [](ResultType const &) {
                } },
            m_value);
        return value();
    }

    ResultType const &must()
    {
        return must([](ErrorType const &error) -> R { return error; });
    }

    template<typename Catch>
    Result &on_error(Catch const &catch_)
    {
        if (std::holds_alternative<ErrorType>(m_value)) {
            m_value = catch_(*this);
        }
        return *this;
    }

private:
    std::variant<ResultType, ErrorType> m_value {};
};

#define TRY_EVAL(...)                 \
    ({                                \
        auto _result = (__VA_ARGS__); \
        if (!_result.has_value()) {   \
            return _result.error();   \
        }                             \
        _result.value();              \
    })

#define TRY_EVAL_FORWARD(E, ...)      \
    ({                                \
        auto _result = (__VA_ARGS__); \
        if (!_result.has_value()) {   \
            return (E);               \
        }                             \
        _result.value();              \
    })

template<typename ErrorType = LibCError>
class [[nodiscard]] Error {
public:
    using E = Error;
    Error(ErrorType const &error)
        : m_error(error)
    {
    }

    Error(ErrorType &&error)
        : m_error(std::move(error))
    {
    }

    Error() = default;
    Error(Error &&other) noexcept = default;
    Error(Error const &other) = default;
    ~Error() = default;

    Error   &operator=(Error &&other) noexcept = default;
    Error   &operator=(Error const &other) = default;
    explicit operator bool() const { return !is_error(); }

    ErrorType         &error() { return m_error.value(); }
    [[nodiscard]] bool is_error() const { return m_error.has_value(); }

    template<typename Catch>
    void must(Catch const &catch_)
    {
        if (m_error.has_value()) {
            if (E new_error { catch_(m_error.value()) }; new_error.is_error()) {
                std::println("Aborting: Error::must(): {}", new_error.error());
                exit(1);
            }
        }
    }

    void must()
    {
        if (m_error.has_value()) {
            std::println("Aborting: Error::must(): {}", m_error.value());
            exit(1);
        }
    }

    template<typename Catch>
    void on_error(Catch const &catch_)
    {
        if (m_error.has_value()) {
            catch_(m_error.value());
        }
    }

    template<typename NewError, typename Adapter>
    Error<NewError> adapt(Adapter const &adapter)
    {
        if (m_error.has_value()) {
            return Error<NewError> { adapter(m_error.value()) };
        }
        return Error<NewError> {};
    }

protected:
    std::optional<ErrorType> m_error = {};
};

using CError = Error<>;

#define TRY(expr)                   \
    do {                            \
        auto _result = (expr);      \
        if (_result.is_error()) {   \
            return _result.error(); \
        }                           \
    } while (0)

#define TRY_FORWARD(E, expr)      \
    do {                          \
        auto _result = (expr);    \
        if (_result.is_error()) { \
            return (E);           \
        }                         \
    } while (0)

}

#pragma clang diagnostic pop
