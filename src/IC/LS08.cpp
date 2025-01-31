/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "LS08.h"
#include "Circuit/UtilityDevice.h"

namespace Simul {

LS08::LS08()
    : Device("74LS08")
{
    for (auto ix = 0; ix < 4; ++ix) {
        auto *gate = add_component<AndGate>();
        A[ix] = gate->A1;
        B[ix] = gate->A2;
        Y[ix] = gate->Y;
    }
}

void LS08_test(Board &board)
{
    board.circuit.name = "LS08 Test";
    board.circuit.add_component<LS08>();
    auto *ls08 = board.add_device<LS08, DIP<14, Orientation::North>>(10, 6);
    std::array<TieDown *, 8> in {};
    std::array<Pin*,4> outputs {};
    for (auto gate = 0; gate < 4; ++gate) {
        in[2*gate] = board.circuit.add_component<TieDown>(PinState::Low);
        in[2*gate+1] = board.circuit.add_component<TieDown>(PinState::Low);
        ls08->A[gate]->feed = in[2*gate]->Y;
        ls08->B[gate]->feed = in[2*gate+1]->Y;
        outputs[gate] = ls08->Y[gate];
    }
    auto S = board.add_package<DIPSwitch<8, Orientation::North>>(2, 3);
    connect(in, S);
    auto L = board.add_package<LEDArray<4, Orientation::North>>(17, 6);
    connect(outputs, L);
}

}
