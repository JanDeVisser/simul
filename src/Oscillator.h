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
    explicit OscillatorIcon(Vector2 pos)
        : Package<1>(pos)
    {
        AbstractPackage::rect = { Package<1>::pin1_tx.x, Package<1>::pin1_tx.y, 4*PITCH, 4*PITCH };
    }

    void render() override
    {
        Vector2 center = Vector2Add(Package<1>::pin1_tx, { 2*PITCH, 2*PITCH });
        DrawCircleV(center, 2*PITCH, GRAY);
        if (Package<1>::pins[0]->on()) {
            Vector2 points[6] {
                { center.x - 1.5f * PITCH, center.y + PITCH },
                { center.x - PITCH, center.y + PITCH },
                { center.x - PITCH, center.y - PITCH },
                { center.x + PITCH, center.y - PITCH },
                { center.x + PITCH, center.y + PITCH },
                { center.x + 1.5f * PITCH, center.y + PITCH },
            };
            DrawLineStrip(points, 6, BLACK);
        } else {
            Vector2 points[6] {
                { center.x - 1.5f * PITCH, center.y - PITCH },
                { center.x - PITCH, center.y - PITCH },
                { center.x - PITCH, center.y + PITCH },
                { center.x + PITCH, center.y + PITCH },
                { center.x + PITCH, center.y - PITCH },
                { center.x + 1.5f * PITCH, center.y - PITCH },
            };
            DrawLineStrip(points, 6, BLACK);
        }
    }
};

void oscillator_test(Board &board);

template<>
inline void connect(Oscillator *device, OscillatorIcon *package)
{
    package->pins[0] = device->Y;
}

}
