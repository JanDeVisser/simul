/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "LogicGate.h"

namespace Simul {

Inverter::Inverter(std::string const &ref)
    : Device("Inverter", ref)
{
    A = add_pin(1, "A");
    Y = add_pin(2, "Y");
}

void Inverter::simulate(duration)
{
    if (A->state != PinState::Z) {
        Y->state = !A->state;
    }
}

LogicGate::LogicGate(std::string const &name, int inputs, std::string const &ref)
    : Device(name, ref)
{
    assert(inputs > 1);
    A1 = add_pin(1, "A1");
    A2 = add_pin(2, "A2");
    for (auto ix = 2; ix < inputs; ++ix) {
        add_pin(ix, std::format("A{}", ix));
    }
    Y = add_pin(inputs + 1, "Y");
}

void LogicGate::simulate(duration)
{
    auto s = operate(A1->state, A2->state);
    for (auto ix = 2; ix < pins.size() - 1; ++ix) {
        s = operate(s, pins[ix]->state);
    }
    Y->state = finalize(s);
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

NandGate::NandGate(int inputs, std::string const &ref)
    : AndGate("NAND", inputs, ref)
{
}

PinState NandGate::finalize(PinState s)
{
    return !s;
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

NorGate::NorGate(int inputs, std::string const &ref)
    : OrGate("NOR", inputs, ref)
{
}

PinState NorGate::finalize(PinState s)
{
    return !s;
}

XorGate::XorGate(std::string const &ref)
    : Device("XOR", ref)
{
    A1 = add_pin(1, "A1");
    A2 = add_pin(2, "A2");
    Y = add_pin(3, "Y");
}

void XorGate::simulate(duration)
{
    Y->state = A1->state ^ A2->state;
}


}
