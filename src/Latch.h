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

void SRLatch_test(Board&);
void DFlipFlop_test(Board&);

}
