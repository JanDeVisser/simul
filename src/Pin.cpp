/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <cassert>

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

bool Pin::update(duration d)
{
    if (feed) {
        if (feed->new_state != PinState::Z && feed->new_state != new_state) {
            new_state = feed->new_state;
        }
    } else {
        if (on_update) {
            (*on_update)(this, d);
        }
    }
    return state != new_state;
}

bool Pin::on() const
{
    return new_state == PinState::High;
}

bool Pin::off() const
{
    return new_state != PinState::High;
}

void Pin::flip()
{
    new_state = !new_state;
}

}
