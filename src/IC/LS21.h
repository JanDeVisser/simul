/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Graphics.h"
#include "LogicGate.h"

namespace Simul {

struct LS21 : public Device {
    std::array<AndGate *, 2> gates;
    std::array<Pin *, 2>     A {};
    std::array<Pin *, 2>     B {};
    std::array<Pin *, 2>     C {};
    std::array<Pin *, 2>     D {};
    std::array<Pin *, 2>     Y {};

    LS21();
};

void LS21_test(Board &);

template<Orientation O>
inline void connect(LS21 *device, DIP<14, O> *package)
{
    package->pins[0] = device->A[0];
    package->pins[1] = device->B[0];
    package->pins[2] = nullptr; // NC
    package->pins[3] = device->C[0];
    package->pins[4] = device->D[0];
    package->pins[5] = device->Y[0];
    package->pins[7] = device->Y[1];
    package->pins[8] = device->A[1];
    package->pins[9] = device->B[1];
    package->pins[10] = nullptr; // NC
    package->pins[11] = device->C[1];
    package->pins[12] = device->D[1];
}

}
