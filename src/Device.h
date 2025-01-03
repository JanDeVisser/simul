/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cassert>
#include <string>
#include <vector>

#include "Pin.h"

namespace Simul {

using namespace std::chrono_literals;
using duration = std::chrono::high_resolution_clock::duration;

struct Device {
    std::string           name;
    std::string           ref;
    std::vector<Pin *>    pins;
    std::vector<Device *> components;

    explicit Device(std::string name, std::string ref)
        : name(std::move(name))
        , ref(std::move(ref))
    {
    }

    explicit Device(std::string name)
        : name(std::move(name))
    {
    }

    virtual ~Device()
    {
        for (auto *pin : pins) {
            delete pin;
        }
        for (auto *component : components) {
            delete component;
        }
    }

    virtual void simulate(duration ts)
    {
    }

    void simulate_step(duration ts)
    {
        propagate();
        for (auto *component : components) {
            component->simulate_step(ts);
        }
        simulate(ts);
    }

    void propagate()
    {
        for (auto *pin : pins) {
            if (pin->feed && pin->feed->state != PinState::Z) {
                pin->state = pin->feed->state;
            }
        }
    }

    [[nodiscard]] Pin *pin(int nr) const
    {
        for (auto *pin : pins) {
            if (pin->pin_nr == nr) {
                return pin;
            }
        }
        return nullptr;
    }

    Pin *add_pin(int nr, std::string const &pin_name, PinState state = PinState::Z)
    {
        pins.push_back(new Pin { nr, pin_name, state });
        return pins.back();
    }

    template<class D, typename... Args>
    D *add_component(Args &&...args)
    {
        components.push_back(new D { args... });
        return dynamic_cast<D *>(components.back());
    }
};

}
