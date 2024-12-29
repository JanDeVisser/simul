/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Device.h"

namespace Simul {

struct Inverter : public Device {
    Pin *A;
    Pin *Y;

    explicit Inverter(std::string const &ref = "");
    void simulate(duration) override;
};

struct LogicGate : public Device {
    Pin *A1;
    Pin *A2;
    Pin *Y;

    explicit LogicGate(std::string const &name, int inputs = 2, std::string const &ref = "");
    void             simulate(duration) override;
    virtual PinState operate(PinState s1, PinState s2) = 0;
    virtual PinState finalize(PinState s);
};

struct AndGate : public LogicGate {
    explicit AndGate(int inputs = 2, std::string const &ref = "");
    PinState operate(PinState s1, PinState s2) override;

protected:
    explicit AndGate(std::string const &name, int inputs = 2, std::string const &ref = "");
};

struct NandGate : public AndGate {
    explicit NandGate(int inputs = 2, std::string const &ref = "");
    PinState finalize(PinState s) override;
};

struct OrGate : public LogicGate {
    explicit OrGate(int inputs = 2, std::string const &ref = "");
    PinState operate(PinState s1, PinState s2) override;

protected:
    explicit OrGate(std::string const &name, int inputs = 2, std::string const &ref = "");
};

struct NorGate : public OrGate {
    explicit NorGate(int inputs = 2, std::string const &ref = "");
    PinState finalize(PinState s) override;
};

struct XorGate : public Device {
    Pin *a1;
    Pin *a2;
    Pin *y;

    explicit XorGate(std::string const &ref = "");
    void simulate(duration) override;
};

}
