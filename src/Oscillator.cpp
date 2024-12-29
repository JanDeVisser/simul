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
}

void Oscillator::simulate(duration ts)
{
    if (ts - last_pulse > period) {
        Y->state = !Y->state;
        last_pulse = ts;
    }
}
void oscillator_test(Board &board)
{
    board.circuit.name = "Oscillator test";
    auto *oscillator = board.circuit.add_component<Oscillator>(2);
    auto *icon = board.add_package<OscillatorIcon>(Vector2 { 1, 1 });
    connect(oscillator, icon);
    auto L = board.add_package<LEDArray<1, Orientation::North>>(Vector2 { 6, 2 });
    connect(oscillator->Y, L);
}

}
