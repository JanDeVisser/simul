/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "Pin.h"

namespace Simul {

std::ostream &operator<<(std::ostream &os, PinState s)
{
    switch (s) {
    case PinState::High:
        os << "H";
        break;
    case PinState::Low:
        os << "L";
        break;
    case PinState::Z:
        os << "Z";
        break;
    }
    return os;
}

PinState operator!(PinState const &s)
{
    return static_cast<PinState>(5 - static_cast<int>(s));
}

PinState operator&(PinState const &s1, PinState const &s2)
{
    return (static_cast<int>(s1) + static_cast<int>(s2) > 5) ? PinState::High : PinState::Low;
}

PinState operator|(PinState const &s1, PinState const &s2)
{
    return (static_cast<int>(s1) + static_cast<int>(s2) > 0) ? PinState::High : PinState::Low;
}

PinState operator^(PinState const &s1, PinState const &s2)
{
    return (static_cast<int>(s1) + static_cast<int>(s2) == 5) ? PinState::High : PinState::Low;
}

bool Pin::on() const
{
    return state == PinState::High;
}

bool Pin::off() const
{
    return state != PinState::High;
}

void Pin::flip()
{
    state = !state;
}

}
