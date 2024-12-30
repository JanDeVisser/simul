/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "Latch.h"
#include "LogicGate.h"
#include "Oscillator.h"
#include "UtilityDevice.h"

namespace Simul {

SRLatch::SRLatch()
    : Device("SRLatch")
{
    auto *nand1 = add_component<NandGate>();
    auto *nand2 = add_component<NandGate>();
    _S = nand1->A1;
    nand1->A2->feed = nand2->Y;
    _R = nand2->A1;
    nand2->A2->feed = nand1->Y;
    Q = nand1->Y;
    _Q = nand2->Y;
}

DFlipFlop::DFlipFlop()
    : Device("DFlipFlop")
{
    auto *output = add_component<SRLatch>();
    auto *d_input = add_component<SRLatch>();
    auto *a_input = add_component<SRLatch>();
    auto *and_gate = add_component<AndGate>();

    Q = output->Q;
    output->_S->feed = a_input->_Q;
    output->_R->feed = d_input->Q;
    D = d_input->_R;
    CLK = add_pin(1, "CLK");
    a_input->_R->feed = CLK;
    a_input->_S->feed = d_input->_Q;
    d_input->_S->feed = and_gate->Y;
    and_gate->A1->feed = CLK;
    and_gate->A2->feed = a_input->_Q;
}

void SRLatch_test(Board &board)
{
    board.circuit.name = "SRLatch Test";
    auto *latch = board.circuit.add_component<SRLatch>();
    std::array<TieDown *, 2> in {};
    for (auto ix = 0; ix < 2; ++ix) {
        in[ix] = board.circuit.add_component<TieDown>(PinState::Low);
    }
    latch->_S->feed = in[0]->Y;
    latch->_R->feed = in[1]->Y;
    auto S = board.add_package<DIPSwitch<2, Orientation::North>>(Vector2 { 2, 3 });
    connect(in, S);
    auto L = board.add_package<LEDArray<2, Orientation::North>>(Vector2 { 8, 3 });
    connect(std::array<Pin*,2> { latch->Q, latch->_Q }, L);
}

void DFlipFlop_test(Board &board)
{
    board.circuit.name = "D-Flip Flop Test";
    auto *latch = board.circuit.add_component<DFlipFlop>();
    auto *clock = board.circuit.add_component<Oscillator>(1);
    board.add_device<Oscillator,OscillatorIcon>(clock, Vector2 { 1, 1 });
    auto *D = board.circuit.add_component<TieDown>(PinState::Low);
    latch->D->feed = D->Y;
    latch->CLK->feed = clock->Y;
    auto S = board.add_package<DIPSwitch<1, Orientation::West>>(Vector2 { 7, 5 });
    connect(D->Y, S);
    auto L = board.add_package<LEDArray<1, Orientation::North>>(Vector2 { 11, 2 });
    connect(latch->Q, L);
}

}
