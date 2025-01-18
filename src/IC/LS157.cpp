/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "LS157.h"

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
    i0_set->A2->feed = input_1_selected;
    combiner->A1 = i0_set->Y;
    combiner->A2 = i1_set->Y;
    Z = combiner->Y;
}

LS157::LS157()
    : Device("74LS157 - Quad 2 input multiplexer")
{
    auto *s_inverter = add_component<Inverter>();
    S = s_inverter->A;

    auto *input_0_selector = add_component<NorGate>();
    E_ = input_0_selector->A1;
    input_0_selector->A2->feed = S;
    auto *input_1_selector = add_component<NorGate>();
    input_1_selector->A1->feed = E_;
    input_1_selector->A2->feed = s_inverter->Y;
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

}
