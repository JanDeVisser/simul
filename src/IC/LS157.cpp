/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "IC/LS157.h"
#include "UtilityDevice.h"

namespace Simul {

LS157::Channel::Channel(Pin *input_0_selected, Pin *input_1_selected)
    : Device("LS74157 bit processor")
{
    i0_set = add_component<AndGate>();
    i1_set = add_component<AndGate>();
    combiner = add_component<OrGate>();

    I0 = i0_set->A1;
    i0_set->A2->feed = input_0_selected;
    I1 = i1_set->A1;
    i1_set->A2->feed = input_1_selected;
    combiner->A1->feed = i0_set->Y;
    combiner->A2->feed = i1_set->Y;
    Z = combiner->Y;
}

LS157::LS157()
    : Device("74LS157 - Quad 2 input multiplexer")
{
    auto *input_0_selector = add_component<NorGate>();
    E_ = input_0_selector->A1;
    S = input_0_selector->A2;
    auto *input_1_selector = add_component<NorGate>();
    input_1_selector->A1->feed = E_;
    invert(S, input_1_selector->A2);
    Pin *selected0 = input_0_selector->Y;
    Pin *selected1 = input_1_selector->Y;

    for (auto bit = 0; bit < 4; ++bit) {
        auto *proc = add_component<Channel>(selected0, selected1);
        I0[bit] = proc->I0;
        I1[bit] = proc->I1;
        Z[bit] = proc->Z;
        channels[bit] = proc;
    }
}

void LS157_test(Board &board)
{
    board.circuit.name = "LS157 Test";
    auto *ls157 = board.add_device<LS157, DIP<16, Orientation::North>>(16, 8);

    auto *Sw = board.add_package<DIPSwitch<2, Orientation::North>>(7, 1);
    auto  inputs = std::array<Pin *, 2> {};
    auto *S = board.circuit.add_component<TieDown>(PinState::Low);
    inputs[0] = S->Y;
    ls157->S->feed = S->Y;
    auto *E_ = board.circuit.add_component<TieDown>(PinState::High);
    inputs[1] = E_->Y;
    ls157->E_->feed = E_->Y;
    connect(inputs, Sw);
    board.add_text(1, 1, "S");
    board.add_text(12, 1, "L->0, H->1");
    board.add_text(1, 3, "E_");

    auto *A_sw = board.add_package<DIPSwitch<4, Orientation::North>>(7, 7);
    auto *B_sw = board.add_package<DIPSwitch<4, Orientation::North>>(7, 16);
    for (auto bit = 0; bit < 4; ++bit) {
        board.add_text(1, 7 + bit * 2, std::format("I0_{}", bit));
        board.add_text(1, 16 + bit * 2, std::format("I1_{}", bit));
        board.add_text(27, 10 + bit * 2, std::format("Z{}", bit));
    }
    connect(ls157->I0, A_sw);
    connect(ls157->I1, B_sw);

    auto *Z = board.add_package<LEDArray<4, Orientation::North>>(24, 10);
    connect(ls157->Z, Z);
}

}
