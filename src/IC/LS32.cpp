/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "LS32.h"
#include "Circuit/UtilityDevice.h"

namespace Simul {

LS32::LS32()
    : Device("74LS32")
{
    for (auto ix = 0; ix < 4; ++ix) {
        auto *gate = add_component<OrGate>();
        A[ix] = gate->A1;
        B[ix] = gate->A2;
        Y[ix] = gate->Y;
    }
}

void LS32_test(Board &board)
{
    board.circuit.name = "LS32 Test";
    auto *ls32 = board.add_device<LS32, DIP<14, Orientation::North>>(10, 6);
    std::array<TieDown *, 8> in {};
    std::array<Pin*,4> outputs {};
    for (auto gate = 0; gate < 4; ++gate) {
        in[2*gate] = board.circuit.add_component<TieDown>(PinState::Low);
        in[2*gate+1] = board.circuit.add_component<TieDown>(PinState::Low);
        ls32->A[gate]->feed = in[2*gate]->Y;
        ls32->B[gate]->feed = in[2*gate+1]->Y;
        outputs[gate] = ls32->Y[gate];
    }
    auto S = board.add_package<DIPSwitch<8, Orientation::North>>(2, 3);
    connect(in, S);
    auto L = board.add_package<LEDArray<4, Orientation::North>>(17, 6);
    connect(outputs, L);
}

}
