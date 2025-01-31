/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <raylib.h>

#include "Circuit/Circuit.h"
#include "Circuit/Graphics.h"
#include "LS574.h"
#include "Circuit/Latch.h"
#include "Circuit/LogicGate.h"
#include "Circuit/UtilityDevice.h"

namespace Simul {

LS574::Latch::Latch()
    : Device("Single LS574 latch")
{
    OE = add_pin(1, "OE", PinState::Low);
    CLK = add_pin(2, "CLK", PinState::Low);
    D = add_pin(3, "D");
    Q = add_pin(4, "Q", PinState::Z);
    flipflop = add_component<DFlipFlop>();
    flipflop->CLK->feed = CLK;
    flipflop->D->feed = D;
    output = add_component<TriStateBuffer>();
    output->E->feed = OE;
    output->A->feed = flipflop->Q;
    Q = output->Y;
    Y = flipflop->Q;
}

LS574::LS574()
    : Device("74LS574 - Octal edge triggered d type flip flops with 3 state outputs", "74LS574")
{
    OEinv = add_component<Inverter>();
    OE_ = OEinv->A;
    CLK = add_pin(11, "CLK");
    for (auto bit = 0; bit < 8; ++bit) {
        latches[bit] = add_component<Latch>();
        latches[bit]->CLK->feed = CLK;
        latches[bit]->OE->feed = OEinv->Y;
        D[bit] = latches[bit]->D;
        Q[bit] = latches[bit]->Q;
    }
}

struct LatchView : public Package<5> {
    explicit LatchView(Vector2 pin1)
        : Package<5>(pin1)
    {
        rect = { pin1_tx.x, pin1_tx.y, 6*PITCH, 6*PITCH };
    }

    void render() override
    {
        auto p { pin1_tx };
        DrawRectangleLines(p.x + PITCH, p.y + PITCH, 4*PITCH, 4*PITCH, BLACK);
        DrawTriangleLines(
            { p.x + PITCH, p.y + 3.4f * PITCH },
            { p.x + PITCH, p.y + 4.6f * PITCH },
            { p.x + 2 * PITCH, p.y + 4 * PITCH },
            BLACK);
        DrawCircleV({p.x + PITCH, p.y + 2*PITCH }, 0.4f*PITCH, pin_color(pins[0]));
        DrawCircleV({p.x + PITCH, p.y + 3*PITCH }, 0.4f*PITCH, pin_color(pins[1]));
        DrawCircleV({p.x + PITCH, p.y + 4*PITCH }, 0.4f*PITCH, pin_color(pins[2]));
        DrawCircleV({p.x + 5*PITCH, p.y + 2*PITCH }, 0.4f*PITCH, pin_color(pins[3]));
        DrawCircleV({p.x + 5*PITCH, p.y + 4*PITCH }, 0.4f*PITCH, pin_color(pins[4]));
    }
};

template <>
void connect(LS574::Latch *device, LatchView *package)
{
    package->pins[0] = device->OE;
    package->pins[1] = device->D;
    package->pins[2] = device->CLK;
    package->pins[3] = device->Y;
    package->pins[4] = device->Q;
}

void LS574_latch_test(Board &board)
{
    auto  inputs = std::array<Pin *, 3> {};
    auto *OE = board.circuit.add_component<TieDown>(PinState::Low);
    inputs[0] = OE->Y;
    auto *CLK = board.circuit.add_component<TieDown>(PinState::Low);
    inputs[1] = CLK->Y;
    auto *D = board.circuit.add_component<TieDown>(PinState::Low);
    inputs[2] = D->Y;
    auto *S = board.add_package<DIPSwitch<3, Orientation::North>>(1, 3);
    connect(inputs, S);

    auto *latch = board.circuit.add_component<LS574::Latch>();
    latch->OE->feed = OE->Y;
    latch->CLK->feed = CLK->Y;
    latch->D->feed = D->Y;
    board.add_device<DFlipFlop, DFlipFlopIcon>(latch->flipflop, 7, 6);
    board.add_device<TriStateBuffer, TriStateIcon>(latch->output, 15, 6);

    auto *L = board.add_package<LEDArray<1, Orientation::North>>(20, 6);
    connect(latch->Q, L);
}

void LS574_test(Board &board)
{
    board.circuit.name = "LS574 Test";
    auto *ls574 = board.circuit.add_component<LS574>();
    board.add_device<LS574, DIP<20, Orientation::North>>(ls574, 10, 6);

    auto  controls = std::array<Pin *, 2> {};
    auto *OE = board.circuit.add_component<TieDown>(PinState::Low);
    controls[0] = OE->Y;
    auto *CLK = board.circuit.add_component<TieDown>(PinState::Low);
    controls[1] = CLK->Y;
    auto *S = board.add_package<DIPSwitch<2, Orientation::North>>(1, 3);
    connect(controls, S);

    auto *oe_inv = board.circuit.add_component<Inverter>();
    oe_inv->A->feed = OE->Y;
    auto *and_gate = board.circuit.add_component<AndGate>();
    and_gate->A1->feed = CLK->Y;
    and_gate->A2->feed = oe_inv->A;
    ls574->CLK->feed = and_gate->Y;
    ls574->OE_->feed = oe_inv->Y;

    auto d_switches = std::array<Pin *, 8> {};
    auto d_pins = std::array<Pin *, 8> {};
    for (auto bit = 0; bit < 8; ++bit) {
        auto *tiedown = board.circuit.add_component<TieDown>(PinState::Low);
        d_switches[bit] = tiedown->Y;
        ls574->D[bit]->feed = tiedown->Y;
        d_pins[bit] = tiedown->Y;
        board.add_device<LS574::Latch, LatchView>(ls574->latches[bit], 16, 1 + bit*8);
    }
    auto *data_S = board.add_package<DIPSwitch<8, Orientation::North>>(1, 9);
    connect(d_pins, data_S);
    auto *L = board.add_package<LEDArray<8, Orientation::North>>(23, 3);
    connect(ls574->Q, L);
}

}
