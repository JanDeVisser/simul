/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "LogicGate.h"
#include "UtilityDevice.h"

namespace Simul {
Inverter::Inverter(std::string const &ref)
    : Device("Inverter", ref)
{
    A = add_pin(1, "A");
    Y = add_pin(2, "Y");
    simulate_device = [this](Device *, duration d) -> void {
        if (A->new_state != PinState::Z) {
            Y->new_state = !A->new_state;
        } else {
            Y->new_state = PinState::Z;
        }
    };
}

Inverter::Inverter(Pin *in, Pin *out)
    : Inverter()
{
    A->feed = in;
    out->feed = Y;
}

InverterIcon::InverterIcon(Vector2 pos)
    : Package<2>(pos)
{
    AbstractPackage::rect = { Package<2>::pin1_tx.x, Package<2>::pin1_tx.y, 4 * PITCH, 4 * PITCH };
}

void InverterIcon::render()
{
    auto p = Package<2>::pin1_tx;
    DrawTriangleLines(
        { p.x + PITCH / 2, p.y + PITCH },
        { p.x + PITCH / 2, p.y + 3 * PITCH },
        { p.x + 2.5f * PITCH, p.y + 2 * PITCH },
        BLACK);
    DrawCircleLinesV({ p.x + 3 * PITCH, p.y + 2 * PITCH }, 0.4f * PITCH, BLACK);
    DrawCircleV({ p.x + PITCH / 2, p.y + 2 * PITCH }, PITCH / 4, pin_color(Package<2>::pins[0]));
    DrawCircleV({ p.x + 3.75f * PITCH, p.y + 2 * PITCH }, PITCH / 4, pin_color(Package<2>::pins[1]));
}

LogicGate::LogicGate(std::string const &name, int inputs, std::string const &ref)
    : Device(name, ref)
{
    assert(inputs > 1);
    A1 = add_pin(1, "A1");
    simulate_device = [this](Device *, duration d) -> void {
        if (pins.size() == 1) {
            Y->new_state = finalize(A1->new_state);
        }
        auto s = operate(A1->new_state, A2->new_state);
        for (auto ix = 2; ix < pins.size() - 1; ++ix) {
            s = operate(s, pins[ix]->new_state);
        }
        s = finalize(s);
        Y->new_state = s;
    };
    A2 = add_pin(2, "A2");
    for (auto ix = 3; ix <= inputs; ++ix) {
        add_pin(ix, std::format("A{}", ix));
    }
    Y = add_pin(inputs + 1, "Y");
}

PinState LogicGate::finalize(PinState s)
{
    return s;
}

AndGate::AndGate(int inputs, std::string const &ref)
    : AndGate("AND", inputs, ref)
{
}

PinState AndGate::operate(PinState s1, PinState s2)
{
    return s1 & s2;
}

AndGate::AndGate(std::string const &name, int inputs, std::string const &ref)
    : LogicGate(name, inputs, ref)
{
}

LogicIcon::LogicIcon(Vector2 pos)
    : Package<3>(pos)
{
    AbstractPackage::rect = { pin1_tx.x, pin1_tx.y, 6 * PITCH, 6 * PITCH };
}

void LogicIcon::render()
{
    auto p = pin1_tx;
    DrawRectangleLines(p.x + PITCH, p.y + PITCH, 4 * PITCH, 4 * PITCH, BLACK);

    auto inputs = device->pins.size() - 1;
    assert(inputs > 1);
    auto y = 3.0f / (static_cast<float>(inputs) - 1.0f);
    for (auto ix = 0; ix < inputs; ++ix) {
        DrawCircleV({ p.x + PITCH, p.y + (1.5f + static_cast<float>(ix) * y) * PITCH }, PITCH / 4, pin_color(device->pins[ix]));
    }
    DrawCircleV({ p.x + 5 * PITCH, p.y + 3.0f * PITCH }, PITCH / 4, pin_color(dynamic_cast<LogicGate *>(device)->Y));
    DrawTextPro(board->font, label(), Vector2 { p.x + 1.5f * PITCH, p.y + PITCH }, Vector2 { 0, 0 }, 0, 20, 2, BLACK);
    if (neg()) {
        DrawCircleLinesV({ p.x + 5.2f * PITCH, p.y + 3 * PITCH }, 0.4f * PITCH, BLACK);
    }
}

AndIcon::AndIcon(Vector2 pos)
    : LogicIcon(pos)
{
}

NandGate::NandGate(int inputs, std::string const &ref)
    : AndGate("NAND", inputs, ref)
{
}

PinState NandGate::finalize(PinState s)
{
    return !s;
}

NandIcon::NandIcon(Vector2 pos)
    : AndIcon(pos)
{
}

OrGate::OrGate(int inputs, std::string const &ref)
    : OrGate("OR", inputs, ref)
{
}

PinState OrGate::operate(PinState s1, PinState s2)
{
    return s1 | s2;
}

OrGate::OrGate(std::string const &name, int inputs, std::string const &ref)
    : LogicGate(name, inputs, ref)
{
}

OrIcon::OrIcon(Vector2 pos)
    : LogicIcon(pos)
{
}

NorGate::NorGate(int inputs, std::string const &ref)
    : OrGate("NOR", inputs, ref)
{
}

PinState NorGate::finalize(PinState s)
{
    return !s;
}

NorIcon::NorIcon(Vector2 pos)
    : OrIcon(pos)
{
}

XorGate::XorGate(std::string const &ref)
    : XorGate("XOR", ref)
{
}

XorGate::XorGate(std::string const &name, std::string const &ref)
    : LogicGate(name, 2, ref)
{
}

PinState XorGate::operate(PinState s1, PinState s2)
{
    return s1 ^ s2;
}

XNorGate::XNorGate(std::string const &ref)
    : XorGate("XNOR", ref)
{
}

PinState XNorGate::finalize(PinState s)
{
    return !s;
}

XorIcon::XorIcon(Vector2 pos)
    : LogicIcon(pos)
{
}

TriStateBuffer::TriStateBuffer(std::string const &ref)
    : Device("Tri-state buffer", ref)
{
    A = add_pin(1, "A");
    E = add_pin(1, "E", PinState::Low);
    Y = add_pin(2, "Y");
    simulate_device = [this](Device *, duration d) -> void {
        if (E->new_state == PinState::High) {
            Y->new_driving = true;
            Y->new_state = A->new_state;
        } else {
            Y->new_driving = false;
        }
    };
}

TriStateIcon::TriStateIcon(Vector2 pos)
    : Package<3>(pos)
{
    AbstractPackage::rect = { pin1_tx.x, pin1_tx.y, 4 * PITCH, 4 * PITCH };
}

void TriStateIcon::render()
{
    auto p = pin1_tx;
    DrawTriangleLines(
        { p.x + PITCH, p.y + PITCH },
        { p.x + PITCH, p.y + 3 * PITCH },
        { p.x + 3 * PITCH, p.y + 2 * PITCH },
        BLACK);
    DrawLine(p.x + 2 * PITCH, p.y + 1.5f * PITCH, p.x + 2 * PITCH, p.y + PITCH / 2, BLACK);
    DrawCircleV({ p.x + PITCH, p.y + 2 * PITCH }, PITCH / 4, pin_color(pins[0]));
    DrawCircleV({ p.x + 2 * PITCH, p.y + 1.5f * PITCH }, PITCH / 4, pin_color(pins[1]));
    DrawCircleV({ p.x + 3 * PITCH, p.y + 2 * PITCH }, PITCH / 4, pin_color(pins[2]));
}

void LogicGate_test(Board &board)
{
    board.circuit.name = "Logic Gate Test";
    std::array<Pin *, 5> outputs {};

    auto *inverter = board.circuit.add_component<Inverter>();
    board.add_device<Inverter, InverterIcon>(inverter, 10, 2);
    std::array<TieDown *, 2> in {};
    in[0] = board.circuit.add_component<TieDown>(PinState::Low);
    in[1] = board.circuit.add_component<TieDown>(PinState::Low);
    inverter->A->feed = in[0]->Y;
    outputs[0] = inverter->Y;

    {
        auto *and_gate = board.circuit.add_component<AndGate>();
        board.add_device<AndGate, AndIcon>(and_gate, 10, 6);
        and_gate->A1->feed = in[0]->Y;
        and_gate->A2->feed = in[1]->Y;
        outputs[1] = and_gate->Y;
    }
    {
        auto *or_gate = board.circuit.add_component<OrGate>();
        board.add_device<OrGate, OrIcon>(or_gate, 10, 10);
        or_gate->A1->feed = in[0]->Y;
        or_gate->A2->feed = in[1]->Y;
        outputs[2] = or_gate->Y;
    }
    {
        auto *xor_gate = board.circuit.add_component<XorGate>();
        board.add_device<XorGate, XorIcon>(xor_gate, 10, 14);
        xor_gate->A1->feed = in[0]->Y;
        xor_gate->A2->feed = in[1]->Y;
        outputs[3] = xor_gate->Y;
    }
    {
        auto *tristate = board.circuit.add_component<TriStateBuffer>();
        board.add_device<TriStateBuffer, TriStateIcon>(tristate, 10, 18);
        tristate->A->feed = in[0]->Y;
        tristate->E->feed = in[1]->Y;
        outputs[4] = tristate->Y;
    }
    auto S = board.add_package<DIPSwitch<2, Orientation::North>>(2, 9);
    connect(in, S);
    auto L = board.add_package<LEDArray<5, Orientation::North>>(17, 7);
    connect(outputs, L);
}

}
