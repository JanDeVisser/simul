/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Device.h"
#include "Graphics.h"

namespace Simul {

struct TestPoint : public Device {
    Pin *T;

    explicit TestPoint(std::string const &ref = "");
    [[nodiscard]] bool on() const;
};

struct TieDown : public Device {
    Pin *Y;

    explicit TieDown(PinState state = PinState::Low, std::string const &ref = "");
    [[nodiscard]] bool on() const;
};

template<typename P, size_t S>
requires std::derived_from<P, Package<S>>
void connect(std::array<TieDown *, S> device, P *package)
{
    for (auto ix = 0; ix < S; ++ix) {
        package->pins[ix] = device[ix]->Y;
    }
}

}
