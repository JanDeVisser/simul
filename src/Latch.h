/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Device.h"
#include "Graphics.h"

namespace Simul {

struct SRLatch : public Device {
    Pin *_S;
    Pin *_R;
    Pin *Q;
    Pin *_Q;

    SRLatch();
};

struct DFlipFlop : public Device {
    Pin *CLK;
    Pin *D;
    Pin *Q;

    DFlipFlop();
};

struct DFlipFlopIcon : Package<3> {
    explicit DFlipFlopIcon(Vector2 pos);
    void render() override;
    void handle_input() override;
};

template<>
inline void connect(DFlipFlop *device, DFlipFlopIcon *package)
{
    package->pins[0] = device->D;
    package->pins[1] = device->CLK;
    package->pins[2] = device->Q;
}

void SRLatch_test(Board&);
void DFlipFlop_test(Board&);

}
