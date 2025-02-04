/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Device.h"
#include "Graphics.h"

namespace Simul {

struct Oscillator : public Device {
    using OscillatorCallback = std::function<void(Oscillator *)>;

    duration                          period;
    duration                          last_pulse {};
    Pin                              *Y;
    std::optional<OscillatorCallback> on_high;
    std::optional<OscillatorCallback> on_low;

    explicit Oscillator(int frequency);
};

struct BurstTrigger : public Device {
    duration burst;
    duration last_pulse {};
    Pin     *A;
    Pin     *Y;

    explicit BurstTrigger(duration burst);
};

struct OscillatorIcon : Package<1> {
    explicit OscillatorIcon(Vector2 pos);
    void render() override;
    void handle_input() override;
};

void oscillator_test(Board &board);

template<>
inline void connect(Oscillator *device, OscillatorIcon *package)
{
    package->pins[0] = device->Y;
}

}
