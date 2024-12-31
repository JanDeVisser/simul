/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>

#include "Device.h"
#include "Graphics.h"
#include "Pin.h"

namespace Simul {

struct LS139 : public Device {
    std::array<Pin *, 2> G {};
    std::array<Pin *, 2> A {};
    std::array<Pin *, 2> B {};
    std::array<Pin *, 2> Y0 {};
    std::array<Pin *, 2> Y1 {};
    std::array<Pin *, 2> Y2 {};
    std::array<Pin *, 2> Y3 {};

    LS139();
};

void LS139_test(Board &);

template<Orientation O>
inline void connect(LS139 *device, DIP<16, O> *package)
{
    package->pins[0] = device->G[0];
    package->pins[1] = device->A[0];
    package->pins[2] = device->B[0];
    package->pins[3] = device->Y0[0];
    package->pins[4] = device->Y1[0];
    package->pins[5] = device->Y2[0];
    package->pins[6] = device->Y3[0];
    package->pins[8] = device->Y3[1];
    package->pins[9] = device->Y2[1];
    package->pins[10] = device->Y1[1];
    package->pins[11] = device->Y0[1];
    package->pins[12] = device->B[1];
    package->pins[13] = device->A[1];
    package->pins[14] = device->G[1];
}

}
