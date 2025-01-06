/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Circuit.h"
#include "ControlBus.h"
#include "GP_Register.h"
#include "Graphics.h"
#include "LS04.h"
#include "LS08.h"
#include "LS138.h"
#include "LS245.h"
#include "LS32.h"
#include "LS377.h"
#include "System.h"
#include "UtilityDevice.h"

namespace Simul {

struct Monitor : public Device {
    ControlBus          *bus;
    std::array<Pin *, 8> SW1;
    std::array<Pin *, 8> SW2;
    LS138               *U1;
    LS245               *U3;
    LS245               *U4;
    LS32                *U6;
    LS08                *U7;
    Pin                 *GET_;

    Monitor(System &system);
};

Card make_Monitor(System &system);

}
