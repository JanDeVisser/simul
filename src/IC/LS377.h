/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>

#include "Device.h"
#include "Graphics.h"
#include "Latch.h"
#include "LogicGate.h"
#include "Pin.h"

namespace Simul {

struct LS377 : public Device {
    struct Latch : public Device {
        Pin *D;
        Pin *Q;
        Pin *E;
        Pin *CLK;

        AndGate   *Dand;
        AndGate   *feedback;
        OrGate    *combine;
        DFlipFlop *flipflop;

        Latch();
    };

    Pin                   *E_ {};
    Inverter              *Einv;
    Pin                   *CLK {};
    std::array<Pin *, 8>   D {};
    std::array<Latch *, 8> latches {};
    std::array<Pin *, 8>   Q {};

    LS377();
};

void LS377_test(Board &);
void LS377_latch_test(Board &);

template<Orientation O>
inline void connect(LS377 *device, DIP<20, O> *package)
{
    package->pins[0] = device->E_;
    package->pins[1] = device->Q[0];
    package->pins[2] = device->D[0];
    package->pins[3] = device->D[1];
    package->pins[4] = device->Q[1];
    package->pins[5] = device->Q[2];
    package->pins[6] = device->D[2];
    package->pins[7] = device->D[3];
    package->pins[8] = device->Q[3];
    package->pins[10] = device->CLK;
    package->pins[11] = device->Q[4];
    package->pins[12] = device->D[4];
    package->pins[13] = device->D[5];
    package->pins[14] = device->Q[5];
    package->pins[15] = device->Q[6];
    package->pins[16] = device->D[6];
    package->pins[17] = device->D[7];
    package->pins[18] = device->Q[7];
}

}
