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
    duration period;
    duration last_pulse {};
    Pin     *Y;

    explicit Oscillator(int frequency);
    void simulate(duration ts) override;
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
