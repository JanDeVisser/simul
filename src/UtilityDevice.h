/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Device.h"
#include "Graphics.h"

namespace Simul {

struct TieDown : public Device {
    Pin *Y;

    explicit TieDown(PinState state = PinState::Low, std::string const &ref = "");
    [[nodiscard]] bool on() const;
};

template<int T, bool HighLow = true>
struct Switch : public Device {
    Pin                    *Y;
    duration                pulse_length {};
    std::optional<duration> last_pulse {};

    explicit Switch(std::string const &ref = "")
        : Device(ref)
    {
        Y = add_pin(1, "Y", PinState::Low);
        pulse_length = std::chrono::milliseconds { T };
        simulate_device = [this](Device *, duration d) -> void {
            if (Y->on()) {
                if (!last_pulse) {
                    last_pulse = d;
                } else if (d - *last_pulse > pulse_length) {
                    Y->new_state = PinState::Low;
                    last_pulse.reset();
                }
            } else if (last_pulse) {
                last_pulse.reset();
            }
        };
    }
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
