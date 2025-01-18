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

SRLatch::SRLatch(int inputs)
    : Device("S/R Latch")
{
    assert(inputs > 0);
    S_Gate = add_component<NandGate>(inputs + 1, "S_Gate");
    R_Gate = add_component<NandGate>(inputs + 1, "R_Gate");
    Q = S_Gate->Y;
    Q_ = R_Gate->Y;
    S_ = S_Gate->A1;
    S_->state = PinState::Low;
    S_Gate->A2->state = PinState::Low;
    R_ = R_Gate->A1;
    R_->state = PinState::Low;
    R_Gate->A2->state = PinState::High;
    Q->state = PinState::High;
    Q_->state = PinState::Low;
    S_Gate->A2->feed = Q_;
    R_Gate->A2->feed = Q;
}

void SRLatch::test_setup(Circuit &)
{
    S_->state = PinState::Low;
    R_->state = PinState::High;
}

void SRLatch::test_run(Circuit &)
{
    assert(Q->state != Q_->state);
    auto q = Q->state;
    S_->state = PinState::High;
    R_->state = PinState::Low;
    std::this_thread::sleep_for(5ms);
    assert(Q->state != q);
}

DFlipFlop::DFlipFlop()
    : Device("DFlipFlop")
{
    output = add_component<SRLatch>(2);
    d_input = add_component<SRLatch>(2);
    a_input = add_component<SRLatch>(2);

    Q = output->Q;
    Q_ = output->Q_;
    Q->state = PinState::Low;
    Q_->state = PinState::High;

    a_input->S_->feed = d_input->Q_;
    a_input->Q->state = PinState::Low;
    a_input->Q_->state = PinState::High;
    SET_ = a_input->S_Gate->pin(3);
    CLK = a_input->R_;
    CLR_ = a_input->R_Gate->pin(3);
    SET_->state = PinState::High;
    CLR_->state = PinState::High;

    d_input->S_->feed = CLK;
    d_input->S_Gate->pin(3)->feed = a_input->Q_;
    d_input->Q->state = PinState::High;
    d_input->Q_->state = PinState::Low;
    D = d_input->R_;
    d_input->R_Gate->pin(3)->feed = CLR_;

    output->S_->feed = a_input->Q_;
    output->S_Gate->pin(3)->feed = SET_;
    output->R_->feed = d_input->Q;
    output->R_Gate->pin(3)->feed = CLR_;
}

void DFlipFlop::test_run(Circuit &circuit)
{
    CLK->state = PinState::Low;
    D->state = PinState::High;
    circuit.yield();
    CLK->state = PinState::High;
    circuit.yield();
    assert(Q->on());
    assert(Q_->off());
    CLK->state = PinState::Low;
    circuit.yield();
    D->state = PinState::Low;
    CLK->state = PinState::High;
    circuit.yield();
    assert(Q->off());
    assert(Q_->on());
}

TFlipFlop::TFlipFlop()
    : Device("TFlipFlop")
{
    flip_flop = add_component<DFlipFlop>();
    toggle = add_component<XorGate>();

    Q = flip_flop->Q;
    Q_ = flip_flop->Q_;
    CLK = flip_flop->CLK;
    SET_ = flip_flop->SET_;
    CLR_ = flip_flop->CLR_;
    T = toggle->A1;
    toggle->A2->feed = Q;
    flip_flop->D->feed = toggle->Y;
}

void SRLatch_test(Board &board)
{
    board.circuit.name = "SRLatch Test";
    auto                    *latch = board.circuit.add_component<SRLatch>();
    std::array<TieDown *, 2> in {};
    for (auto ix = 0; ix < 2; ++ix) {
        in[ix] = board.circuit.add_component<TieDown>(PinState::Low);
    }
    latch->S_->feed = in[0]->Y;
    latch->R_->feed = in[1]->Y;
    auto S = board.add_package<DIPSwitch<2, Orientation::North>>(2, 3);
    connect(in, S);
    auto L = board.add_package<LEDArray<2, Orientation::North>>(8, 3);
    connect(std::array<Pin *, 2> { latch->Q, latch->Q_ }, L);
}

DFlipFlopIcon::DFlipFlopIcon(Vector2 pos)
    : Package<3>(pos)
{
    AbstractPackage::rect = { pin1_tx.x, pin1_tx.y, 6 * PITCH, 6 * PITCH };
}

void DFlipFlopIcon::render()
{
    auto p = pin1_tx;
    DrawRectangleLines(p.x + PITCH, p.y + PITCH, 4 * PITCH, 4 * PITCH, BLACK);
    DrawTriangleLines(
        { p.x + PITCH, p.y + 3.4f * PITCH },
        { p.x + PITCH, p.y + 4.6f * PITCH },
        { p.x + 2 * PITCH, p.y + 4 * PITCH },
        BLACK);
    DrawCircleV({ p.x + PITCH, p.y + 2 * PITCH }, 0.4f * PITCH, pin_color(pins[0]));
    DrawCircleV({ p.x + PITCH, p.y + 4 * PITCH }, 0.4f * PITCH, pin_color(pins[1]));
    DrawCircleV({ p.x + 5 * PITCH, p.y + 3 * PITCH }, 0.4f * PITCH, pin_color(pins[2]));
}

void DFlipFlopIcon::handle_input()
{
}

void DFlipFlop_test(Board &board)
{
    board.circuit.name = "D-Flip Flop Test";
    auto *latch = board.add_device<DFlipFlop, DFlipFlopIcon>(8, 1);
    auto *clock = board.circuit.add_component<Oscillator>(1);
    board.add_device<Oscillator, OscillatorIcon>(clock, 1, 5);
    auto *D = board.circuit.add_component<TieDown>(PinState::Low);
    latch->D->feed = D->Y;
    latch->CLK->feed = clock->Y;
    auto S = board.add_package<DIPSwitch<1, Orientation::North>>(1, 1);
    connect(D->Y, S);
    auto L = board.add_package<LEDArray<1, Orientation::North>>(16, 3);
    connect(latch->Q, L);
}

JKFlipFlop::JKFlipFlop()
    : Device("J/K Flip-flop with set and clear")
{
    J_gate = add_component<NandGate>(3);
    K_gate = add_component<NandGate>(3);
    secondary = add_component<SRLatch>();
    clr = add_component<AndGate>();
    set = add_component<AndGate>();

    Q = secondary->Q;
    Q_ = secondary->Q_;
    assert(Q == secondary->Q);

    CLK = J_gate->A1;
    J = J_gate->A2;
    J_gate->pin(3)->feed = secondary->Q_;

    SET_ = set->A1;
    SET_->state = PinState::High;
    set->A2->feed = J_gate->Y;

    K_gate->A1->feed = CLK;
    K = K_gate->A2;
    K_gate->pin(3)->feed = secondary->Q;

    CLR_ = clr->A1;
    CLR_->state = PinState::High;
    clr->A2->feed = J_gate->Y;

    secondary->S_->feed = set->Y;
    secondary->R_->feed = clr->Y;
}

void JKFlipFlop::test_setup(Circuit &circuit)
{
}

void JKFlipFlop::test_run(Circuit &circuit)
{
    CLR_->state = SET_->state = PinState::High;
    CLK->state = PinState::Low;
    J->state = PinState::High;
    K->state = PinState::Low;
    circuit.yield();

    CLK->state = PinState::High;
    circuit.yield();

    assert(Q->on());
    CLK->state = PinState::Low;
    circuit.yield();

    assert(Q->on());
    J->state = PinState::High;
    K->state = PinState::High;
    CLK->state = PinState::High;
    circuit.yield();

    assert(Q->off());
    CLK->state = PinState::Low;
    circuit.yield();

    assert(Q->off());
    CLK->state = PinState::High;
    circuit.yield();

    assert(Q->on());
    SET_->state = PinState::Low;
    circuit.yield();

    assert(Q->on());
    CLR_->state = PinState::Low;
    circuit.yield();

    assert(Q->off());
}

JKFlipFlopIcon::JKFlipFlopIcon(Vector2 pos)
    : Package<5>(pos)
{
    AbstractPackage::rect = { pin1_tx.x, pin1_tx.y, 6 * PITCH, 6 * PITCH };
}

void JKFlipFlopIcon::render()
{
    auto p = pin1_tx;
    DrawRectangleLines(p.x + PITCH, p.y + PITCH, 4 * PITCH, 4 * PITCH, BLACK);
    DrawTriangleLines(
        { p.x + PITCH, p.y + 2.4f * PITCH },
        { p.x + PITCH, p.y + 3.6f * PITCH },
        { p.x + 2 * PITCH, p.y + 3 * PITCH },
        BLACK);
    DrawCircleV({ p.x + PITCH, p.y + 2 * PITCH }, 0.4f * PITCH, pin_color(pins[0]));
    DrawCircleV({ p.x + PITCH, p.y + 4 * PITCH }, 0.4f * PITCH, pin_color(pins[1]));
    DrawCircleV({ p.x + PITCH, p.y + 3 * PITCH }, 0.4f * PITCH, pin_color(pins[2]));
    DrawCircleV({ p.x + 5 * PITCH, p.y + 2 * PITCH }, 0.4f * PITCH, pin_color(pins[3]));
    DrawCircleV({ p.x + 5 * PITCH, p.y + 4 * PITCH }, 0.4f * PITCH, pin_color(pins[4]));
}

void JKFlipFlopIcon::handle_input()
{
}

TFlipFlopIcon::TFlipFlopIcon(Vector2 pos)
    : Package<6>(pos)
{
    AbstractPackage::rect = { pin1_tx.x, pin1_tx.y, 6 * PITCH, 6 * PITCH };
}

void TFlipFlopIcon::render()
{
    auto p = pin1_tx;
    DrawRectangleLines(p.x + PITCH, p.y + PITCH, 4 * PITCH, 4 * PITCH, BLACK);
    DrawTriangleLines(
        { p.x + PITCH, p.y + 2.4f * PITCH },
        { p.x + PITCH, p.y + 3.6f * PITCH },
        { p.x + 2 * PITCH, p.y + 3 * PITCH },
        BLACK);
    DrawCircleV({ p.x + PITCH, p.y + 2 * PITCH }, 0.4f * PITCH, pin_color(pins[0]));
    DrawCircleV({ p.x + PITCH, p.y + 3 * PITCH }, 0.4f * PITCH, pin_color(pins[1]));
    DrawCircleV({ p.x + 3 * PITCH, p.y + PITCH }, 0.4f * PITCH, pin_color(pins[2]));
    DrawCircleV({ p.x + 3 * PITCH, p.y + 5 * PITCH }, 0.4f * PITCH, pin_color(pins[3]));
    DrawCircleV({ p.x + 5 * PITCH, p.y + 2 * PITCH }, 0.4f * PITCH, pin_color(pins[4]));
    DrawCircleV({ p.x + 5 * PITCH, p.y + 4 * PITCH }, 0.4f * PITCH, pin_color(pins[5]));
}

void TFlipFlopIcon::handle_input()
{
}

}
