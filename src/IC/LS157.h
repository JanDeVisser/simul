/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Circuit/Device.h"
#include "Circuit/LogicGate.h"
#include "Circuit/Pin.h"

namespace Simul {

struct LS157 : public Device {
    struct Channel : public Device {
        Pin     *I0;
        Pin     *I1;
        Pin     *Z;
        AndGate *i0_set;
        AndGate *i1_set;
        OrGate  *combiner;

        Channel(Pin *input_0_selected, Pin *input_1_selected);
    };

    Pin                     *E_ { nullptr };
    Pin                     *S { nullptr };
    std::array<Pin *, 4>     I0 {};
    std::array<Pin *, 4>     I1 {};
    std::array<Pin *, 4>     Z {};
    std::array<Channel *, 4> channels {};

    LS157();
};

void LS157_test(Board &);

template<Orientation O>
inline void connect(LS157 *device, DIP<16, O> *package)
{
    package->pins[0] = device->S;
    package->pins[1] = device->I0[0];
    package->pins[2] = device->I1[3];
    package->pins[3] = device->Z[0];
    package->pins[4] = device->I0[1];
    package->pins[5] = device->I1[1];
    package->pins[6] = device->Z[1];
    package->pins[8] = device->Z[3];
    package->pins[9] = device->I1[3];
    package->pins[10] = device->I0[3];
    package->pins[11] = device->Z[2];
    package->pins[12] = device->I1[2];
    package->pins[13] = device->I0[2];
    package->pins[14] = device->E_;
}

}
