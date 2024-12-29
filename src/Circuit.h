/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Device.h"

namespace Simul {

struct Circuit : public Device {
    Pin *GND {};
    Pin *VCC {};

    explicit Circuit(std::string const& name = "")
        : Device(name)
    {
        GND = add_pin(1, "GND");
        GND->state = PinState::Low;
        VCC = add_pin(1, "VCC");
        VCC->state = PinState::High;
    }
};

}
