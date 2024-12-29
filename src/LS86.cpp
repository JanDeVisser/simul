/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "LS86.h"
#include "UtilityDevice.h"

namespace Simul {

LS86::LS86()
    : Device("74LS86")
{
    for (auto ix = 0; ix < 4; ++ix) {
        auto *gate = add_component<XorGate>();
        A[ix] = gate->A1;
        B[ix] = gate->A2;
        Y[ix] = gate->Y;
    }
}

void LS86_test(Board &board)
{
    board.circuit.name = "LS86 Test";
    auto *ls86 = board.circuit.add_component<LS86>();
    board.add_device<LS86, DIP<14, Orientation::North>>(ls86, Vector2 { 10, 6 });
    std::array<TieDown *, 8> in {};
    std::array<Pin*,4> outputs {};
    for (auto gate = 0; gate < 4; ++gate) {
        in[2*gate] = board.circuit.add_component<TieDown>(PinState::Low);
        in[2*gate+1] = board.circuit.add_component<TieDown>(PinState::Low);
        ls86->A[gate]->feed = in[2*gate]->Y;
        ls86->B[gate]->feed = in[2*gate+1]->Y;
        outputs[gate] = ls86->Y[gate];
    }
    auto S = board.add_package<DIPSwitch<8, Orientation::North>>(Vector2 { 2, 3 });
    connect(in, S);
    auto L = board.add_package<LEDArray<4, Orientation::North>>(Vector2 { 17, 6 });
    connect(outputs, L);
}

}
