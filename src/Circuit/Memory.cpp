/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "Memory.h"
#include "UtilityDevice.h"

namespace Simul {

void memory_test(Board &board)
{
    board.circuit.name = "Memory Test";
    auto *sram = board.add_device<SRAM_LY62256, DIP<28, Orientation::North>>(20, 10);

    auto  inputs = std::array<Pin *, 3> {};
    auto *CE_ = board.circuit.add_component<TieDown>(PinState::High);
    inputs[0] = CE_->Y;
    auto *WE_ = board.circuit.add_component<TieDown>(PinState::High);
    inputs[1] = WE_->Y;
    auto *OE_ = board.circuit.add_component<TieDown>(PinState::High);
    inputs[2] = OE_->Y;
    auto *S = board.add_package<DIPSwitch<3, Orientation::North>>(10, 1);
    connect(inputs, S);
    board.add_text(1, 1, "CE_");
    board.add_text(1, 3, "WE_");
    board.add_text(1, 5, "OE_");

    sram->CE_->feed = CE_->Y;
    sram->WE_->feed = WE_->Y;
    sram->OE_->feed = OE_->Y;

    auto *D_sw = board.add_package<TriStateSwitch<8, Orientation::North>>(10, 9);
    connect(sram->D, D_sw);
    board.add_text(1, 9, "D0-D7");
    auto *A_sw = board.add_package<DIPSwitch<15, Orientation::North>>(10, 27);
    board.add_text(1, 27, "A0-A15");
    connect(sram->A, A_sw);
    auto *D_led = board.add_package<LEDArray<8, Orientation::North>>(35, 9);
    connect(sram->D, D_led);
    board.add_text(37, 9, "D0-D7");
}

}
