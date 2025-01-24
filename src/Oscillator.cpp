/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "Oscillator.h"

namespace Simul {

Oscillator::Oscillator(int frequency)
    : Device("Oscillator")
    , period(std::chrono::duration_cast<std::chrono::nanoseconds>(1s) / frequency)
{
    Y = add_pin(1, "Phi");
    Y->state = PinState::Low;
    Y->on_update = [this](Pin *, duration d) -> void {
        if (d - last_pulse > period) {
            Y->new_state = !Y->new_state;
            last_pulse = d;
        }
    };
}

OscillatorIcon::OscillatorIcon(Vector2 pos)
    : Package<1>(pos)
{
    AbstractPackage::rect = { Package<1>::pin1_tx.x, Package<1>::pin1_tx.y, 4 * PITCH, 4 * PITCH };
}

void OscillatorIcon::render()
{
    Vector2 center = Vector2Add(Package<1>::pin1_tx, { 2 * PITCH, 2 * PITCH });
    DrawCircleV(center, 2 * PITCH, GRAY);
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

void OscillatorIcon::handle_input()
{
}

BurstTrigger::BurstTrigger(duration b)
    : Device("BurstTrigger")
    , burst(b)
{
    A = add_pin(1, "A", PinState::Low);
    Y = add_pin(2, "Y", PinState::Low);
    simulate_device = [this](Device *, duration d) -> void {
        if (A->on()) {
            if (Y->on()) {
                if (d - last_pulse > burst) {
                    Y->new_state = PinState::Low;
                }
            } else if (A->state != A->new_state) {
                Y->new_state = PinState::High;
                last_pulse = d;
            }
        } else {
            Y->new_state = PinState::Low;
        }
    };
}

void oscillator_test(Board &board)
{
    board.circuit.name = "Oscillator test";
    auto *oscillator = board.circuit.add_component<Oscillator>(2);
    board.add_device<Oscillator, OscillatorIcon>(oscillator, 1, 1);
    auto L = board.add_package<LEDArray<1, Orientation::North>>(6, 2);
    connect(oscillator->Y, L);
}

}
