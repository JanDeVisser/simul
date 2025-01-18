/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Circuit.h"
#include "Device.h"
#include "Latch.h"

namespace Simul {

struct LS193 : public Device {
    struct Latch : public Device {
        Pin       *D;
        Pin       *Q;
        Pin       *Q_;
        TFlipFlop *latch;
        NandGate  *SetNand;
        NandGate  *LoadNand;
        OrGate    *ResetOr;
        AndGate   *clockBorrowAnd { nullptr };
        AndGate   *clockCarryAnd { nullptr };
        OrGate    *ClockOr;

        Latch(LS193 *chip, int bit);
    };

    std::array<Pin *, 4>   D {};
    std::array<Pin *, 4>   Q {};
    std::array<Latch *, 4> latches {};
    Pin                   *CLR;
    Pin                   *CLR_;
    Pin                   *Load_;
    Pin                   *Load;
    Pin                   *Up;
    Pin                   *Up_;
    Pin                   *Down;
    Pin                   *Down_;
    Pin                   *BO_;
    Pin                   *CO_;
    NandGate              *BONand;
    NandGate              *CONand;

    LS193();

    void test_setup(Circuit &circuit) override;
    void test_run(Circuit &circuit) override;
};

struct LS193_Bit0 : public Device {
    Pin       *D;
    Pin       *Q;
    Pin       *CLR;
    Pin       *Load_;
    Pin       *Up;
    Pin       *Down;
    Pin       *BO_;
    Pin       *CO_;
    TFlipFlop *latch;
    NandGate  *SetNand;
    NandGate  *LoadNand;
    OrGate    *ResetOr;
    OrGate    *ClockOr;

    LS193_Bit0();
};

template<Orientation O>
inline void connect(LS193 *device, DIP<16, O> *package)
{
    package->pins[0] = device->D[1];
    package->pins[1] = device->Q[1];
    package->pins[2] = device->Q[1];
    package->pins[3] = device->Down;
    package->pins[4] = device->Up;
    package->pins[5] = device->Q[2];
    package->pins[6] = device->Q[3];
    package->pins[8] = device->D[3];
    package->pins[9] = device->D[2];
    package->pins[10] = device->Load_;
    package->pins[11] = device->CO_;
    package->pins[12] = device->BO_;
    package->pins[13] = device->CLR;
    package->pins[14] = device->D[0];
}

}
