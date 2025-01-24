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

#include "IC/LS04.h"
#include "IC/LS08.h"
#include "IC/LS138.h"
#include "IC/LS245.h"
#include "IC/LS32.h"
#include "IC/LS377.h"

namespace Simul {

struct Mem_Register : public Device {
    ControlBus    *bus;
    LS138         *U1;
    LS138         *U2;
    LS32          *U3;
    LS04          *U4;
    LS08          *U5;
    LS245         *U6;
    LS377         *U7;
    LS377         *U8;
    SRAM_LY62256  *U9;
    EEPROM_28C256 *U10;

    Pin *DataGet_;
    Pin *DataPut_;
    Pin *AddrPut_;
    Pin *Data_;
    Pin *DataClk_;

    explicit Mem_Register(System &system);
};

Card make_Mem_Register(System &system);

}
