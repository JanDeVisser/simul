/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "Device.h"
#include "Graphics.h"
#include "LogicGate.h"

namespace Simul {

Device::~Device()
{
    for (auto *component : components) {
        delete component;
    }
}

Pin *Device::pin(int nr) const
{
    for (auto *pin : pins) {
        if (pin->pin_nr == nr) {
            return pin;
        }
    }
    return nullptr;
}

Pin *Device::add_pin(int nr, std::string const &pin_name, PinState state)
{
    auto *ret = Circuit::the().allocate_pin(nr, pin_name, state);
    pins.push_back(ret);
    return ret;
}

void Device::invert(Pin *in, Pin *out)
{
    add_component<Inverter>(in, out);
}

}
