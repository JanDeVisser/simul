/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Graphics.h"
#include "LogicGate.h"

namespace Simul {

struct LS32 : public Device {
    std::array<Pin *, 4> A {};
    std::array<Pin *, 4> B {};
    std::array<Pin *, 4> Y {};

    LS32();
};

void LS32_test(Board&);

template<Orientation O>
inline void connect(LS32 *device, DIP<14, O> *package)
{
    package->pins[0] = device->A[0];
    package->pins[1] = device->B[0];
    package->pins[2] = device->Y[0];
    package->pins[3] = device->A[1];
    package->pins[4] = device->B[1];
    package->pins[5] = device->Y[1];
    package->pins[7] = device->Y[2];
    package->pins[8] = device->A[2];
    package->pins[9] = device->B[2];
    package->pins[10] = device->Y[3];
    package->pins[11] = device->A[3];
    package->pins[12] = device->B[3];
}

}
