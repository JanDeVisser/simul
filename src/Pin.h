/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <iostream>
#include <string>

namespace Simul {

enum class PinState {
    Low = 0,
    High = 5,
    Z = -1,
};

std::ostream &operator<<(std::ostream &os, PinState s);
PinState      operator!(PinState const &s);
PinState      operator&(PinState const &s1, PinState const &s2);
PinState      operator|(PinState const &s1, PinState const &s2);
PinState      operator^(PinState const &s1, PinState const &s2);

struct Pin {
    int         pin_nr;
    std::string name;
    PinState    state { PinState::Z };
    Pin        *feed { nullptr };
    Pin        *drive { nullptr };

    [[nodiscard]] bool on() const;
    [[nodiscard]] bool off() const;
    void flip();
};

}
