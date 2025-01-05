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
    auto S = board.add_package<DIPSwitch<2, Orientation::North>>(2, 3);
    connect(in, S);
    auto L = board.add_package<LEDArray<2, Orientation::North>>(8, 3);
    connect(std::array<Pin*,2> { latch->Q, latch->_Q }, L);
}

DFlipFlopIcon::DFlipFlopIcon(Vector2 pos)
    : Package<3>(pos)
{
    AbstractPackage::rect = { pin1_tx.x, pin1_tx.y, 6*PITCH, 6*PITCH };
}

void DFlipFlopIcon::render()
{
    auto p = pin1_tx;
    DrawRectangleLines(p.x + PITCH, p.y + PITCH, 4*PITCH, 4*PITCH, BLACK);
    DrawTriangleLines(
        { p.x + PITCH, p.y + 3.4f * PITCH },
        { p.x + PITCH, p.y + 4.6f * PITCH },
        { p.x + 2 * PITCH, p.y + 4 * PITCH },
        BLACK);
    DrawCircleV({p.x + PITCH, p.y + 2*PITCH }, 0.4f*PITCH, pin_color(pins[0]));
    DrawCircleV({p.x + PITCH, p.y + 4*PITCH }, 0.4f*PITCH, pin_color(pins[1]));
    DrawCircleV({p.x + 5*PITCH, p.y + 3*PITCH }, 0.4f*PITCH, pin_color(pins[2]));
}

void DFlipFlopIcon::handle_input()
{
}

void DFlipFlop_test(Board &board)
{
    board.circuit.name = "D-Flip Flop Test";
    auto *latch = board.add_device<DFlipFlop,DFlipFlopIcon>(8, 1);
    auto *clock = board.circuit.add_component<Oscillator>(1);
    board.add_device<Oscillator,OscillatorIcon>(clock, 1, 5);
    auto *D = board.circuit.add_component<TieDown>(PinState::Low);
    latch->D->feed = D->Y;
    latch->CLK->feed = clock->Y;
    auto S = board.add_package<DIPSwitch<1, Orientation::North>>(1, 1);
    connect(D->Y, S);
    auto L = board.add_package<LEDArray<1, Orientation::North>>(16, 3);
    connect(latch->Q, L);
}

}
