/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Circuit/Circuit.h"
#include "Circuit/Graphics.h"
#include "Circuit/UtilityDevice.h"
#include "ControlBus.h"
#include "IC/LS04.h"
#include "IC/LS08.h"
#include "IC/LS138.h"
#include "IC/LS245.h"
#include "IC/LS32.h"
#include "IC/LS377.h"
#include "System.h"

namespace Simul {

struct Monitor : public Device {
    ControlBus          *bus;
    std::array<Pin *, 8> SW1 {};
    std::array<Pin *, 8> SW2 {};
    LS138               *U1;
    LS245               *U3;
    LS245               *U4;
    LS32                *U6;
    LS08                *U7;
    Pin                 *GET_;

    explicit Monitor(System &system);
};

struct Card make_Monitor(System &system);

}
