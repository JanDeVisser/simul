/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <raylib.h>

#include "Circuit.h"
#include "Graphics.h"
#include "LS377.h"
#include "Latch.h"
#include "LogicGate.h"
#include "Oscillator.h"
#include "UtilityDevice.h"

namespace Simul {

LS377::Latch::Latch()
    : Device("LS377 Latch")
{
    flipflop = add_component<DFlipFlop>();
    Dand = add_component<AndGate>();
    feedback = add_component<AndGate>();
    combine = add_component<OrGate>();

    CLK = flipflop->CLK;
    E = Dand->A1;
    D = Dand->A2;
    invert(E, feedback->A1);
    feedback->A2->feed = flipflop->Q;
    combine->A1->feed = Dand->Y;
    combine->A2->feed = feedback->Y;
    flipflop->D->feed = combine->Y;
    flipflop->SET_->state = PinState::High;
    flipflop->CLR_->state = PinState::High;
    Q = flipflop->Q;
}

LS377::LS377()
    : Device("74LS377 - Octal D-Type Flip-Flop with Common Enable and Clock", "74LS377")
{
    Einv = add_component<Inverter>();
    E_ = Einv->A;
    CLK = add_pin(11, "CLK");
    for (auto bit = 0; bit < 8; ++bit) {
        latches[bit] = add_component<Latch>();
        latches[bit]->E->feed = Einv->Y;
        latches[bit]->CLK->feed = CLK;
        D[bit] = latches[bit]->D;
        Q[bit] = latches[bit]->Q;
    }
}

struct LatchView : public Package<4> {
    explicit LatchView(Vector2 pin1)
        : Package<4>(pin1)
    {
        rect = { pin1_tx.x, pin1_tx.y, 6 * PITCH, 6 * PITCH };
    }

    void render() override
    {
        auto p { pin1_tx };
        DrawRectangleLines(p.x + PITCH, p.y + PITCH, 4 * PITCH, 4 * PITCH, BLACK);
        DrawTriangleLines(
            { p.x + PITCH, p.y + 3.4f * PITCH },
            { p.x + PITCH, p.y + 4.6f * PITCH },
            { p.x + 2 * PITCH, p.y + 4 * PITCH },
            BLACK);
        DrawCircleV({ p.x + PITCH, p.y + 2 * PITCH }, 0.4f * PITCH, pin_color(pins[0]));
        DrawCircleV({ p.x + PITCH, p.y + 3 * PITCH }, 0.4f * PITCH, pin_color(pins[1]));
        DrawCircleV({ p.x + PITCH, p.y + 4 * PITCH }, 0.4f * PITCH, pin_color(pins[2]));
        DrawCircleV({ p.x + 5 * PITCH, p.y + 3 * PITCH }, 0.4f * PITCH, pin_color(pins[3]));
    }
};

template<>
void connect(LS377::Latch *device, LatchView *package)
{
    package->pins[0] = device->E;
    package->pins[1] = device->D;
    package->pins[2] = device->CLK;
    package->pins[3] = device->Q;
}

void LS377_latch_test(Board &board)
{
    auto  inputs = std::array<Pin *, 3> {};
    auto *E = board.circuit.add_component<TieDown>(PinState::Low);
    inputs[0] = E->Y;
    auto *CLK = board.circuit.add_component<TieDown>(PinState::Low);
    inputs[1] = CLK->Y;
    auto *D = board.circuit.add_component<TieDown>(PinState::Low);
    inputs[2] = D->Y;
    auto *S = board.add_package<DIPSwitch<3, Orientation::North>>(5, 5);
    board.add_text(1, 5, "E");
    board.add_text(1, 7, "CLK");
    board.add_text(1, 9, "D");
    connect(inputs, S);

    auto inv = board.circuit.add_component<Inverter>();
    inv->A->feed = E->Y;

    auto *latch = board.circuit.add_component<LS377::Latch>();
    latch->E->feed = E->Y;
    latch->CLK->feed = CLK->Y;
    latch->D->feed = D->Y;
    board.add_device<AndGate, AndIcon>(latch->Dand, 12, 1);
    board.add_device<AndGate, AndIcon>(latch->feedback, 19, 1);
    board.add_device<OrGate, OrIcon>(latch->combine, 15, 8);
    board.add_device<DFlipFlop, DFlipFlopIcon>(latch->flipflop, 15, 15);

    auto *L = board.add_package<LEDArray<1, Orientation::North>>(27, 10);
    connect(latch->Q, L);
}

void LS377_test(Board &board)
{
    board.circuit.name = "LS377 Test";
    auto *ls377 = board.circuit.add_component<LS377>();
    board.add_device<LS377, DIP<20, Orientation::North>>(ls377, 10, 6);

    auto  controls = std::array<Pin *, 2> {};
    auto *E = board.circuit.add_component<TieDown>(PinState::High);
    controls[0] = E->Y;
    auto *CLK = board.circuit.add_component<TieDown>(PinState::Low);
    controls[1] = CLK->Y;
    auto *S = board.add_package<DIPSwitch<2, Orientation::North>>(1, 3);
    connect(controls, S);

    ls377->CLK->feed = CLK->Y;
    ls377->E_->feed = E->Y;

    auto d_switches = std::array<Pin *, 8> {};
    auto d_pins = std::array<Pin *, 8> {};
    for (auto bit = 0; bit < 8; ++bit) {
        auto *tiedown = board.circuit.add_component<TieDown>(PinState::Low);
        d_switches[bit] = tiedown->Y;
        ls377->D[bit]->feed = tiedown->Y;
        d_pins[bit] = tiedown->Y;
        board.add_device<LS377::Latch, LatchView>(ls377->latches[bit], 16, 1 + bit * 8);
    }
    auto *data_S = board.add_package<DIPSwitch<8, Orientation::North>>(1, 9);
    connect(d_pins, data_S);
    auto *L = board.add_package<LEDArray<8, Orientation::North>>(23, 3);
    connect(ls377->Q, L);
}

}
