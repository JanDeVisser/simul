/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Device.h"
#include "Pin.h"
#include "Graphics.h"
#include "System.h"
#include "UtilityDevice.h"

namespace Simul {

struct ControlBus : public Device {
    Pin                      *CLK;
    Pin                      *XDATA_;
    Pin                      *XADDR_;
    Pin                      *IO_;
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
};

void bus_label(Board &board, int op, std::string const& label);

template<size_t Bits>
void set_pins(std::array<Pin *, Bits> pins, uint8_t value)
{
    for (auto ix = 0; ix < Bits; ++ix) {
        pins[ix]->state = (value & 0x01) ? PinState::High : PinState::Low;
        value >>= 1;
    }
}

ControlBus * make_backplane(System &system);

}
