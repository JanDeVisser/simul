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

struct LS245 : public Device {
    struct Channel : public Device {
        TristatePin    *A;
        TristatePin    *B;
        Pin            *AE;
        Pin            *BE;
        Pin            *DIR;
        TriStateBuffer *Abuf;
        TriStateBuffer *Bbuf;
        Channel();
    };

    Pin                     *OE_ {};
    Pin                     *DIR {};
    std::array<Pin *, 8>     A {};
    std::array<Pin *, 8>     B {};
    Inverter                *OEinv;
    AndGate                 *ASide;
    NorGate                 *BSide;
    std::array<Channel *, 8> channels {};

    LS245();
};

void LS245_channel_test(Board &board);
void LS245_test(Board &);

template<Orientation O>
inline void connect(LS245 *device, DIP<20, O> *package)
{
    package->pins[0] = device->DIR;
    package->pins[1] = device->A[0];
    package->pins[2] = device->A[1];
    package->pins[3] = device->A[2];
    package->pins[4] = device->A[3];
    package->pins[5] = device->A[4];
    package->pins[6] = device->A[5];
    package->pins[7] = device->A[6];
    package->pins[8] = device->A[7];
    package->pins[10] = device->B[7];
    package->pins[11] = device->B[6];
    package->pins[12] = device->B[5];
    package->pins[13] = device->B[4];
    package->pins[14] = device->B[3];
    package->pins[15] = device->B[2];
    package->pins[16] = device->B[1];
    package->pins[17] = device->B[0];
    package->pins[18] = device->OE_;
}

}
