/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>

#include "Circuit/Device.h"
#include "Circuit/Graphics.h"
#include "Circuit/Pin.h"

namespace Simul {

struct LS138 : public Device {
    Pin * A {};
    Pin * B {};
    Pin * C {};
    Pin * G2A {};
    Pin * G2B {};
    Pin * G1 {};
    std::array<Pin *, 8> Y {};

    LS138();
};

void ls138_test(Board &);

template<Orientation O>
inline void connect(LS138 *device, DIP<16, O> *package)
{
    package->pins[0] = device->A;
    package->pins[1] = device->B;
    package->pins[2] = device->C;
    package->pins[3] = device->G2A;
    package->pins[4] = device->G2B;
    package->pins[5] = device->G1;
    package->pins[6] = device->Y[7];
    package->pins[8] = device->Y[6];
    package->pins[9] = device->Y[5];
    package->pins[10] = device->Y[4];
    package->pins[11] = device->Y[3];
    package->pins[12] = device->Y[2];
    package->pins[13] = device->Y[1];
    package->pins[14] = device->Y[0];
}

}
