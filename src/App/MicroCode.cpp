/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "MicroCode.h"
#include <Lib/FileBuffer.h>
#include <Lib/Grammar/Parser.h>
#include <Lib/GrammarParser/GrammarParser.h>
#include <Lib/Unescape.h>

namespace Simul {

using namespace Lib;

#include "microcode.grammar"

struct MCParser {
    bool                       log { false };
    std::vector<MicroCodeStep> steps {};

    void startup(std::string_view buffer)
    {
    }

    void cleanup() const
    {
    }
};

Result<std::vector<MicroCodeStep>, std::string> parse_microcode(std::string const &file_name)
{
    GrammarParser gp { microcode_grammar };
    Grammar       grammar {};
    if (auto e = gp.parse(grammar); e.is_error()) {
        return std::format("{} Error parsing grammar", gp.lexer.location);
    }
    bool             log { false };
    Parser<MCParser> p { grammar };
    p.log = log;

    std::vector<FileBuffer> buffers;
    auto                    parse_file = [&buffers, &p, log](std::string_view const &file_name) -> Error<std::string> {
        auto unescapify = [](char *buffer, size_t size) -> size_t {
            return unescape(buffer, size);
        };
        if (auto fb = FileBuffer::from_file_filter(file_name, unescapify); fb.is_error()) {
            return std::format("Error opening {}: {} ={}=", file_name, fb.error().to_string(), fs::current_path().string());
        } else {
            buffers.emplace_back(std::move(fb.value()));
            p.parse(buffers.back().contents(), file_name).must();
            return {};
        }
    };
    if (auto e = parse_file(file_name); e.is_error()) {
        return e.error();
    }
    return std::move(p.impl.steps);
}

}

using namespace Simul;
using namespace Lib;

using P = Parser<MCParser>;

extern "C" {

[[maybe_unused]] void set_action(P *parser)
{
    auto &step = parser->impl.steps.emplace_back();
    if (parser->last_token.text == "D") {
        step.action = MicroCodeAction::XData;
        step.payload.emplace<Transfer>();
    } else if (parser->last_token.text == "A") {
        step.action = MicroCodeAction::XAddr;
        step.payload.emplace<Transfer>();
    } else if (parser->last_token.text == "M") {
        step.action = MicroCodeAction::SetMem;
        step.payload.emplace<MemBlock>();
    } else if (parser->last_token.text == "S") {
        step.action = MicroCodeAction::Monitor;
        step.payload.emplace<MonitorValue>();
    } else {
        UNREACHABLE();
    }
}

[[maybe_unused]] void set_get_reg(P *parser)
{
    auto &step = parser->impl.steps.back();
    if (parser->last_token.kind.tag() == KindTag::Number) {
        if (auto conv = parser->last_token.as<uint8_t>(); conv.has_value()) {
            std::get<Transfer>(step.payload).get_from = conv.value() & 0x0F;
        } else {
            UNREACHABLE();
        }
    } else {
        if (auto reg_maybe = Register_from_name(parser->last_token.text); reg_maybe) {
            std::get<Transfer>(step.payload).get_from = static_cast<uint8_t>(reg_maybe.value()) & 0x0F;
        } else {
            UNREACHABLE();
        }
    }
}

[[maybe_unused]] void set_put_reg(P *parser)
{
    auto &step = parser->impl.steps.back();
    if (parser->last_token.kind.tag() == KindTag::Number) {
        if (auto conv = parser->last_token.as<uint8_t>(); conv.has_value()) {
            std::get<Transfer>(step.payload).put_to = conv.value() & 0x0F;
        } else {
            UNREACHABLE();
        }
    } else {
        if (auto reg_maybe = Register_from_name(parser->last_token.text); reg_maybe) {
            std::get<Transfer>(step.payload).put_to = static_cast<uint8_t>(reg_maybe.value()) & 0x0F;
        } else {
            UNREACHABLE();
        }
    }
}

[[maybe_unused]] void set_op_bits(P *parser)
{
    auto &step = parser->impl.steps.back();
    if (parser->last_token.kind.tag() == KindTag::Number) {
        if (auto conv = parser->last_token.as<uint8_t>(); conv.has_value()) {
            std::get<Transfer>(step.payload).op_bits = conv.value() & 0x0F;
        } else {
            UNREACHABLE();
        }
    } else {
        UNREACHABLE();
    }
}

[[maybe_unused]] void set_address(P *parser)
{
    auto &step = parser->impl.steps.back();
    if (parser->last_token.kind.tag() == KindTag::Number) {
        if (auto conv = parser->last_token.as<uint8_t>(); conv.has_value()) {
            std::get<MemBlock>(step.payload).address = conv.value() & 0xFFFF;
        } else {
            UNREACHABLE();
        }
    } else {
        UNREACHABLE();
    }
}

[[maybe_unused]] void append_value(P *parser)
{
    auto &step = parser->impl.steps.back();
    if (parser->last_token.kind.tag() == KindTag::Number) {
        if (auto conv = parser->last_token.as<uint8_t>(); conv.has_value()) {
            std::get<MemBlock>(step.payload).bytes.emplace_back(conv.value() & 0xFF);
        } else {
            UNREACHABLE();
        }
    } else {
        UNREACHABLE();
    }
}
[[maybe_unused]] void set_d_value(P *parser)
{
    auto &step = parser->impl.steps.back();
    if (parser->last_token.kind.tag() == KindTag::Number) {
        if (auto conv = parser->last_token.as<uint8_t>(); conv.has_value()) {
            std::get<MonitorValue>(step.payload).d = conv.value() & 0xFF;
        } else {
            UNREACHABLE();
        }
    } else {
        UNREACHABLE();
    }
}

[[maybe_unused]] void set_a_value(P *parser)
{
    auto &step = parser->impl.steps.back();
    if (parser->last_token.kind.tag() == KindTag::Number) {
        if (auto conv = parser->last_token.as<uint8_t>(); conv.has_value()) {
            std::get<MonitorValue>(step.payload).a = conv.value() & 0xFF;
        } else {
            UNREACHABLE();
        }
    } else {
        UNREACHABLE();
    }
}
}
