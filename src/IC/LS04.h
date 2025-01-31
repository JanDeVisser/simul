/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Circuit/Graphics.h"
#include "Circuit/LogicGate.h"

namespace Simul {

struct LS04 : public Device {
    std::array<Pin *, 6> A {};
    std::array<Pin *, 6> Y {};

    LS04();
};

void ls04_test(Board &);

template<Orientation O>
inline void connect(LS04 *ls04, DIP<14, O> *package)
{
    package->pins[0] = ls04->A[0];
    package->pins[1] = ls04->Y[0];
    package->pins[2] = ls04->A[1];
    package->pins[3] = ls04->Y[1];
    package->pins[4] = ls04->A[2];
    package->pins[5] = ls04->Y[2];
    package->pins[7] = ls04->Y[3];
    package->pins[8] = ls04->A[3];
    package->pins[9] = ls04->Y[4];
    package->pins[10] = ls04->A[4];
    package->pins[11] = ls04->Y[5];
    package->pins[12] = ls04->A[5];
}

}
