/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "LS02.h"
#include "UtilityDevice.h"

namespace Simul {

LS02::LS02()
    : Device("74LS02 - Quadruple 2-Input Positive NOR Gates")
{
    for (auto ix = 0; ix < 4; ++ix) {
        gates[ix] = add_component<NorGate>();
        A[ix] = gates[ix]->A1;
        B[ix] = gates[ix]->A2;
        Y[ix] = gates[ix]->Y;
    }
}

void LS02_test(Board &board)
{
    board.circuit.name = "LS02 Test";
    board.circuit.add_component<LS02>();
    auto                *ls02 = board.add_device<LS02, DIP<14, Orientation::North>>(10, 3);
    std::array<Pin *, 8> in {};
    for (auto gate = 0; gate < 4; ++gate) {
        in[2 * gate] = ls02->A[gate];
        in[2 * gate + 1] = ls02->B[gate];
        board.add_text(1, (gate * 4) + 1, std::format("A{}", gate));
        board.add_text(1, (gate * 4) + 3, std::format("B{}", gate));
    }
    switches<8>(board, 4, 1, in);
    leds<4>(board, 18, 5, ls02->Y);
}

}
