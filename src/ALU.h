/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "ControlBus.h"
#include "Device.h"
#include "Graphics.h"
#include "Memory.h"
#include "System.h"

#include "IC/LS00.h"
#include "IC/LS02.h"
#include "IC/LS04.h"
#include "IC/LS08.h"
#include "IC/LS138.h"
#include "IC/LS157.h"
#include "IC/LS21.h"
#include "IC/LS245.h"
#include "IC/LS32.h"
#include "IC/LS377.h"
#include "IC/LS382.h"

namespace Simul {

struct ALU : public Device {
    ControlBus          *bus;
    LS377               *U1;
    LS245               *U2;
    LS382               *U3;
    LS382               *U4;
    LS245               *U5;
    LS138               *U6;
    LS138               *U7;
    LS377               *U8;
    LS245               *U9;
    LS08                *U10;
    LS02                *U11;
    LS21                *U12;
    LS377               *U13;
    LS157               *U14;
    LS157               *U15;
    LS245               *U16;
    LS08                *U17;
    LS00                *U18;
    LS32                *U19;
    LS04                *U20;
    LS245               *U21;
    Pin                 *Shift_;
    Pin                 *Shift;
    Pin                 *LHS_;
    Pin                 *RHS_;
    Pin                 *Res_;
    Pin                 *Flags_;
    Pin                 *CFlag;
    Pin                 *OFlag;
    Pin                 *ZFlag;
    Pin                 *C_out;
    Pin                 *OVR;
    Pin                 *C;
    Pin                 *O;
    Pin                 *Z;
    std::array<Pin *, 8> F {};

    explicit ALU(System &system);
};

Card make_ALU(System &system);

}
