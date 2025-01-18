/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "ControlBus.h"
#include "GP_Register.h"
#include "Circuit.h"
#include "Graphics.h"
#include "IC/LS04.h"
#include "IC/LS08.h"
#include "IC/LS138.h"
#include "IC/LS245.h"
#include "IC/LS32.h"
#include "IC/LS377.h"
#include "System.h"
#include "UtilityDevice.h"

namespace Simul {

struct GP_Register : public Device {
    int reg_no {0};
    ControlBus *bus;
    LS138 *U1;
    LS138 *U2;
    LS245 *U3;
    LS377 *U4;
    LS04 *U5;
    LS32 *U6;
    LS08 *U7;
    LS32 *U8;
    Pin *PUT_;
    Pin *GET_;
    Pin *IOIn;
    Pin *IOOut;
    Pin *In_;
    Pin *Out_;

    GP_Register(System &system, int reg_no);
};

Card make_GP_Register(System &system, int reg_no);

}
