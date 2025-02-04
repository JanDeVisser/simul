/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <cctype>
#include <charconv>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>

#include <Lib/Result.h>
#include <Lib/Unescape.h>

namespace Lib {

Result<std::optional<std::string>, UnescapeError> unescape(std::string_view s)
{
    if (s.find('\\') == std::string_view::npos) {
        return std::optional<std::string> {};
    }
    std::string           ret;
    bool                  esc = false;
    std::optional<size_t> hexcode_start = {};
    size_t                tix = 0;

    ret.reserve(s.length());
    for (auto ix = 0; ix < s.length(); ++ix) {
        auto ch = s[ix];
        if (hexcode_start) {
            if (isxdigit(ch)) {
                continue;
            }
            char result {};
            auto str = s.substr(*hexcode_start, ix);
            auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);

            if (ec == std::errc::invalid_argument) {
                return UnescapeError::NumberFormat;
            } else if (ec == std::errc::result_out_of_range) {
                return UnescapeError::IntOutOfRange;
            }
            // FIXME make sure that result fits in a char
            ret.push_back((char) result);
            hexcode_start = {};
            continue;
        }
        if (!esc && ch == '\\') {
            esc = true;
            continue;
        }
        auto escaped = ch;
        if (esc) {
            esc = false;
            switch (ch) {
            case 'x':
            case 'X':
                hexcode_start = ix + 1;
                continue;
            case 'n':
                escaped = '\n';
                break;
            case 't':
                escaped = '\t';
                break;
            case 'r':
                escaped = '\r';
                break;
            default:
                escaped = ch;
                break;
            }
        }
        ret.push_back(escaped);
    }
    if (esc || hexcode_start) {
        return UnescapeError::EscapeSyntax;
    }
    return ret;
}

size_t unescape(char *buffer, size_t size)
{
    size_t dst = 0;
    for (auto ix = 0; ix < size - 1;) {
        char ch = buffer[ix++];
        if (ch == '\\' && ix < size - 1) {
            ch = buffer[ix++];
            switch (ch) {
            case 'n':
                ch = '\n';
                break;
            case 't':
                ch = '\t';
                break;
            case 'r':
                ch = '\r';
                break;
            default:
                break;
            }
        }
        buffer[dst++] = ch;
    }
    buffer[dst] = 0;
    return dst;
}

}
