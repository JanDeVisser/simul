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
    using Handler = std::function<void(Device *, duration d)>;
    std::string            name;
    std::string            ref;
    std::vector<Pin *>     pins;
    std::vector<Device *>  components;
    Device                *parent { nullptr };
    std::optional<Handler> simulate_device {};

    explicit Device(std::string name, std::string ref = "")
        : name(std::move(name))
        , ref(std::move(ref))
    {
    }

    virtual ~Device();

    [[nodiscard]] Pin *pin(int nr) const;
    Pin               *add_pin(int nr, std::string const &pin_name, PinState state = PinState::Z);
    void               invert(Pin *in, Pin *out);

    template<class D, typename... Args>
        requires std::derived_from<D, Device>
    D *add_component(Args &&...args)
    {
        auto *device = dynamic_cast<Device *>(new D { args... });
        device->parent = this;
        components.push_back(device);
        return dynamic_cast<D *>(components.back());
    }

    virtual void test_setup(struct Circuit &)
    {
    }

    virtual void test_run(struct Circuit &)
    {
    }
};

}
