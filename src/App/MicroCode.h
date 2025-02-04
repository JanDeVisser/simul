/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <string>

#include <Lib/Result.h>

namespace Simul {

using namespace Lib;

enum class MicroCodeAction {
    XData = 0x00,
    XAddr = 0x01,
    SetMem = 0x02,
    Monitor = 0x03,
};

#define REGISTERS(S) \
    S(A, 0x00)       \
    S(B, 0x01)       \
    S(C, 0x02)       \
    S(D, 0x03)       \
    S(LHS, 0x04)     \
    S(RHS, 0x05)     \
    S(IR, 0x06)      \
    S(Mem, 0x07)     \
    S(PC, 0x08)      \
    S(SP, 0x09)      \
    S(Si, 0x0A)      \
    S(Di, 0x0B)      \
    S(TX, 0x0C)      \
    S(Mon, 0x0D)     \
    S(MemAddr, 0x0E) \
    S(Res, 0x14)     \
    S(Flags, 0x15)

enum class Register : uint8_t {
#undef S
#define S(R, V) R = V,
    REGISTERS(S)
#undef S
};

inline std::string_view Register_name(Register r)
{
    switch (r) {
#undef S
#define S(R, V)       \
    case Register::R: \
        return #R;
        REGISTERS(S)
#undef S
    default:
        UNREACHABLE();
    }
}

inline std::optional<Register> Register_from_name(std::string_view name)
{
#undef S
#define S(R, V)     \
    if (name == #R) \
        return Register::R;
    REGISTERS(S)
#undef S
    return {};
}

struct Transfer {
    uint8_t get_from;
    uint8_t put_to;
    uint8_t op_bits;
};

struct MemBlock {
    size_t               address;
    std::vector<uint8_t> bytes;
};

struct MonitorValue {
    uint8_t d;
    uint8_t a;
};

struct MicroCodeStep {
    MicroCodeAction action;
    std::variant<
        Transfer,
        MemBlock,
        MonitorValue>
        payload;
};

Result<std::vector<MicroCodeStep>, std::string> parse_microcode(std::string const &file_name);

}
