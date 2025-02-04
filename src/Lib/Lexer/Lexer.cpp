//
// Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
//
// SPDX-License-Identifier: MIT
//

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>

#include <Lib/Lexer/Lexer.h>
#include <Lib/Logging.h>
#include <Lib/Result.h>
#include <Lib/Unescape.h>

namespace Lib {

Lexer::Lexer(Config &config, std::string_view source, std::string_view buffer)
    : config(config)
    , source(source)
    , location(buffer)
{
    if (config.Whitespace.on) {
        if (config.Whitespace.ignore_nl) {
            ignored.insert(KindTag::Newline);
        }
        if (config.Whitespace.ignore_ws) {
            ignored.insert(KindTag::Whitespace);
        }
    }
    if (config.Comment.ignore) {
        ignored.insert(KindTag::Comment);
    }
}

Token Lexer::buildToken(uint64_t len, Lib::TokenKind kind)
{
    current = Token { kind, (source.empty() || len == 0) ? "" : source.substr(0, len) };
    current->location = location;
    return *current;
}

void Lexer::advance()
{
    if (!current) {
        UNREACHABLE();
    }
    Token token = *current;
    location.pos += token.text.length();
    source = source.substr(token.text.length());
    current = {};
}

std::optional<Token> Lexer::next()
{
    if (auto t = peek_next(); t) {
        advance();
        return t;
    }
    return {};
}

std::optional<Token> Lexer::peek_next()
{
    while (true) {
        auto t = peek();
        if (!t) {
            return {};
        }
        if (!ignored.contains(t->kind.tag())) {
            return t;
        }
        advance();
    }
}

std::optional<Token> Lexer::peek()
{
    if (current) {
        return current;
    }
    if (exhausted) {
        return {};
    }
    if (source.empty()) {
        exhausted = true;
        return buildToken(0, TokenKind { KindTag::Eof });
    }
    if (source[0] == '\n') {
        auto ret = buildToken(1, TokenKind { KindTag::Newline });
        location.line += 1;
        location.col = 0;
        if (config.Whitespace.on) {
            return ret;
        }
    }
    if (config.Comment.on) {
        if (auto t = config.Comment.scan(*this); t) {
            return t;
        }
    }
    if (source[0] == ' ' || source[0] == '\t') {
        size_t p = 1;
        while (p < source.length() && (source[p] == ' ' || source[p] == '\t')) {
            p += 1;
        }
        auto ret = buildToken(1, TokenKind { KindTag::Whitespace });
        if (config.Whitespace.on) {
            return ret;
        }
    }
    if (config.Number.on) {
        if (auto t = config.Number.scan(*this); t) {
            return t;
        }
    }
    if (config.QString.on) {
        if (auto q = config.QString.quotes.find(source[0]); q != std::string_view::npos) {
            char   quote = config.QString.quotes[q];
            size_t p = 1;
            while (p < source.length() && source[p] != quote) {
                p += (source[p] == '\\') ? 2 : 1;
            }
            if (p < source.length()) {
                p += 1;
            }
            return buildToken(p, TokenKind { KindTag::String, quote });
        }
    }
    if (isalpha(source[0]) || source[0] == '_') {
        size_t p = 1;
        while (p < source.length() && (isalnum(source[p]) || source[p] == '_')) {
            p += 1;
        }
        if (config.Keywords.on) {
            switch (config.Keywords.match(source.substr(0, p))) {
            case Config::Keywords::MatchResult::ExactMatch:
            case Config::Keywords::MatchResult::PrefixAndExact:
                return buildToken(p, TokenKind { KindTag::Keyword, source.substr(0, p) });
            default:
                break;
            }
        }
        if (config.Identifier.on) {
            return buildToken(p, TokenKind { KindTag::Identifier });
        }
    }
    if (config.Keywords.on) {
        size_t matched = 0;
        for (auto l = 1; l < source.length(); ++l) {
            switch (config.Keywords.match(source.substr(0, l))) {
            case Config::Keywords::MatchResult::ExactMatch:
                return buildToken(l, TokenKind { KindTag::Keyword, source.substr(0, l) });
            case Config::Keywords::MatchResult::NoMatch:
                goto default_return;
            case Config::Keywords::MatchResult::Prefix:
                break;
            case Config::Keywords::MatchResult::PrefixAndExact:
                matched = l;
                break;
            case Config::Keywords::MatchResult::MatchLost:
                return buildToken(matched, TokenKind { KindTag::Keyword, source.substr(0, matched) });
            }
        }
    }
default_return:
    return buildToken(1, TokenKind { KindTag::Symbol, source[0] });
}

bool Lexer::accept_keyword(std::string_view keyword)
{
    if (auto t = peek_next(); t) {
        if (t->is(KindTag::Keyword) && t->kind.keyword() == keyword) {
            advance();
            return true;
        }
    }
    return false;
}

Result<Token, Lexer::Error> Lexer::expect_identifier()
{
    if (auto t = peek_next(); t) {
        if (t->is(KindTag::Identifier)) {
            advance();
            return *t;
        }
    }
    return Error::ExpectedIdentifier;
}

std::optional<Token> Lexer::accept_identifier()
{
    if (auto t = peek_next(); t) {
        if (t->is(KindTag::Identifier)) {
            advance();
            return *t;
        }
    }
    return {};
}

Error<Lexer::Error> Lexer::expect_symbol(char symbol)
{
    if (auto t = peek_next(); t) {
        if (t->is(KindTag::Symbol) && t->kind.symbol() == symbol) {
            advance();
            return {};
        }
    }
    return Lexer::Error::ExpectedIdentifier;
}

bool Lexer::accept_symbol(char symbol)
{
    if (auto t = peek_next(); t) {
        if (t->is(KindTag::Symbol) && t->kind.symbol() == symbol) {
            advance();
            return true;
        }
    }
    return false;
}

}
