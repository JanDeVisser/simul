/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "LS00.h"
#include "Circuit/UtilityDevice.h"

namespace Simul {

LS00::LS00()
    : Device("74LS00")
{
    for (auto ix = 0; ix < 4; ++ix) {
        auto *gate = add_component<NandGate>();
        A[ix] = gate->A1;
        B[ix] = gate->A2;
        Y[ix] = gate->Y;
    }
}

void LS00_test(Board &board)
{
    board.circuit.name = "LS00 Test";
    board.circuit.add_component<LS00>();
    auto *ls00 = board.add_device<LS00, DIP<14, Orientation::North>>(10, 6);
    std::array<Pin *, 8> in {};
    std::array<Pin*,4> outputs {};
    for (auto gate = 0; gate < 4; ++gate) {
        in[2*gate] = board.circuit.add_pin(20, "xx", PinState::Low);
        in[2*gate+1] = board.circuit.add_pin(32, "yy", PinState::Low);
        ls00->A[gate]->feed = in[2*gate];
        ls00->B[gate]->feed = in[2*gate+1];
        outputs[gate] = ls00->Y[gate];
    }
    board.connect_package<DIPSwitch<8, Orientation::North>>(in, 2, 3);
    board.connect_package<LEDArray<4, Orientation::North>>(outputs, 17, 6);
}

}
