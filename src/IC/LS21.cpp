/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "LS21.h"
#include "Circuit/UtilityDevice.h"

namespace Simul {

LS21::LS21()
    : Device("74LS21 - Dual 4-Input Positive AND Gates")
{
    for (auto ix = 0; ix < 2; ++ix) {
        gates[ix] = add_component<AndGate>(4);
        A[ix] = gates[ix]->pins[0];
        B[ix] = gates[ix]->pins[1];
        C[ix] = gates[ix]->pins[2];
        D[ix] = gates[ix]->pins[3];
        Y[ix] = gates[ix]->Y;
    }
}

void LS21_test(Board &board)
{
    board.circuit.name = "LS21 Test";
    board.circuit.add_component<LS21>();
    auto                *ls21 = board.add_device<LS21, DIP<14, Orientation::North>>(10, 2);
    std::array<Pin *, 8> in {};
    for (auto gate = 0; gate < 4; ++gate) {
        in[2 * gate] = ls21->A[gate];
        in[2 * gate + 1] = ls21->B[gate];
        in[2 * gate + 2] = ls21->C[gate];
        in[2 * gate + 3] = ls21->D[gate];
        board.add_text(1, (gate * 8) + 1, std::format("A{}", gate));
        board.add_text(1, (gate * 8) + 3, std::format("B{}", gate));
        board.add_text(1, (gate * 8) + 5, std::format("C{}", gate));
        board.add_text(1, (gate * 8) + 7, std::format("D{}", gate));
    }
    switches<8>(board, 1, 4, in);
    leds<2>(board, 18, 6, ls21->Y);
}

}
