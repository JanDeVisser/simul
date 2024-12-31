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

struct LS574 : public Device {
    struct Latch : public Device {
        Pin *OE;
        Pin *CLK;
        Pin *D;
        Pin *Y;
        Pin *Q;

        DFlipFlop      *flipflop;
        TriStateBuffer *output;
        Latch();
    };

    Pin                   *OE_ {};
    Pin                   *CLK {};
    std::array<Pin *, 8>   D {};
    std::array<Pin *, 8>   Q {};
    Inverter              *OEinv;
    std::array<Latch *, 8> latches {};

    LS574();
};

void LS574_latch_test(Board &board);
void LS574_test(Board &);

template<Orientation O>
inline void connect(LS574 *device, DIP<20, O> *package)
{
    package->pins[0] = device->OE_;
    package->pins[1] = device->D[0];
    package->pins[2] = device->D[1];
    package->pins[3] = device->D[2];
    package->pins[4] = device->D[3];
    package->pins[5] = device->D[4];
    package->pins[6] = device->D[5];
    package->pins[7] = device->D[6];
    package->pins[8] = device->D[7];
    package->pins[10] = device->CLK;
    package->pins[11] = device->Q[7];
    package->pins[12] = device->Q[6];
    package->pins[13] = device->Q[5];
    package->pins[14] = device->Q[4];
    package->pins[15] = device->Q[3];
    package->pins[16] = device->Q[2];
    package->pins[17] = device->Q[1];
    package->pins[18] = device->Q[0];
}

}
