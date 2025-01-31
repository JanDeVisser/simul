/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "ControlBus.h"
#include "Circuit/Device.h"
#include "Circuit/Graphics.h"
#include "System.h"

#include "IC/LS00.h"
#include "IC/LS08.h"
#include "IC/LS32.h"
#include "IC/LS138.h"
#include "IC/LS139.h"
#include "IC/LS157.h"
#include "IC/LS193.h"
#include "IC/LS245.h"

namespace Simul {

struct Addr_Register : public Device {
    int reg_no {0};
    ControlBus *bus;
    LS138 *U1;
    LS138 *U2;
    LS138 *U3;
    LS32 *U4;
    LS08 *U5;
    LS32 *U6;
    LS00 *U7;
    LS139 *U8;
    LS193 *U10;
    LS193 *U11;
    LS193 *U12;
    LS193 *U13;
    LS245 *U14;
    LS245 *U15;
    LS245 *U16;
    LS157 *U17;
    LS157 *U18;

    Pin *MSB;
    Pin *Put_;
    Pin *Get_;
    Pin *DPut_;
    Pin *DGet_;
    Pin *APut_;
    Pin *AGet_;
    Pin *LSBPut_;
    Pin *MSBPut_;
    Pin *LSBGet_;
    Pin *MSBGet_;
    Pin *LSBLoad_;
    Pin *MSBLoad_;
    Pin *Decrement;
    Pin *Increment;

    Addr_Register(System &system, int reg_no);
};

Card make_Addr_Register(System &system, int reg_no);

}
