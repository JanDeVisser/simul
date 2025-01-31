/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "LS04.h"
#include "Circuit/UtilityDevice.h"

namespace Simul {

LS04::LS04()
    : Device("74LS04")
{
    for (auto ix = 0; ix < 6; ++ix) {
        auto *inverter = add_component<Inverter>();
        A[ix] = inverter->A;
        Y[ix] = inverter->Y;
    }
}

void ls04_test(Board &board)
{
    board.circuit.name = "LS139 Test";
    auto *ls04 = board.circuit.add_component<LS04>();
    auto U = board.add_package<DIP<14, Orientation::North>>(10, 6);
    connect(ls04, U);
    std::array<TieDown *, 6> in {};
    std::array<Pin*,6> outputs {};
    for (auto inverter = 0; inverter < 6; ++inverter) {
        in[inverter] = board.circuit.add_component<TieDown>(PinState::Low);
        ls04->A[inverter]->feed = in[inverter]->Y;
        outputs[inverter] = ls04->Y[inverter];
    }
    auto S = board.add_package<DIPSwitch<6, Orientation::North>>(2, 3);
    connect(in, S);
    auto L = board.add_package<LEDArray<6, Orientation::North>>(17, 3);
    connect(outputs, L);
}

}
