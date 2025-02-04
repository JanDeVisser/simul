/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <Circuit/Device.h>
#include <Circuit/Graphics.h>
#include <Circuit/Oscillator.h>
#include <Circuit/Pin.h>
#include <Circuit/UtilityDevice.h>

namespace Simul {

struct ControlBus : public Device {
    Pin                      *GND;
    Pin                      *VCC;
    Pin                      *CLK;
    Pin                      *CLK_;
    Pin                      *CLKburst;
    Pin                      *HLT_;
    Pin                      *SUS_;
    Pin                      *XDATA_;
    Pin                      *XADDR_;
    Pin                      *SACK_;
    Pin                      *IO_;
    Pin                      *RST;
    Switch<200>              *clock_switch;
    Oscillator               *oscillator;
    std::array<Pin *, 4>      OP {};
    std::array<Pin *, 4>      PUT {};
    std::array<Pin *, 4>      GET {};
    std::array<Pin *, 24>     controls {};
    std::array<Pin *, 8>      D {};
    std::array<Pin *, 8>      ADDR {};
    std::array<TieDown *, 40> tiedowns {};

    ControlBus();
    void set_op(uint8_t op);
    void set_put(uint8_t op);
    void set_get(uint8_t op);
    void set_data(uint8_t op);
    void set_addr(uint8_t op);
    void data_transfer(uint8_t from, uint8_t to, uint8_t op = 0) const;
    void addr_transfer(uint8_t from, uint8_t to, uint8_t op = 0) const;
    void enable_oscillator();
    void disable_oscillator();
};

void bus_label(Board &board, int op, std::string const &label);

ControlBus *make_backplane(struct System &system);

}
