/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <functional>
#include <iostream>
#include <sstream>
#include <string>

namespace Simul {

using duration = std::chrono::high_resolution_clock::duration;

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
    using Handler = std::function<void(Pin *, duration)>;

    int                    pin_nr { 1 };
    std::string            name {};
    PinState               state { PinState::Z };
    std::optional<Handler> on_change {};
    std::optional<Handler> on_update {};
    Pin                   *feed { nullptr };
    Pin                   *drive { nullptr };
    PinState               new_state { PinState::Z };

    Pin() = default;

    Pin(int pin_nr, std::string name, PinState state)
        : pin_nr(pin_nr)
        , name(std::move(name))
        , state(state)
    {
    }

    bool               update(duration d);
    [[nodiscard]] bool on() const;
    [[nodiscard]] bool off() const;
    void               flip();
};

template<size_t Bits>
void set_pins(std::array<Pin *, Bits> pins, uint8_t value)
{
    for (auto ix = 0; ix < Bits; ++ix) {
        pins[ix]->new_state = (value & 0x01) ? PinState::High : PinState::Low;
        value >>= 1;
    }
}

template<size_t Bits>
uint8_t get_pins(std::array<Pin *, Bits> pins)
{
    uint8_t ret { 0 };
    for (int ix = Bits - 1; ix >= 0; --ix) {
        if (pins[ix]->new_state == PinState::Z) {
            return 0xFF;
        }
        ret = (ret << 1) | ((pins[ix]->new_state == PinState::High) ? 0x01 : 0x00);
    }
    return ret;
}

}

template<>
struct std::formatter<Simul::PinState, char> {
    template<class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext &ctx)
    {
        auto it = ctx.begin();
        if (it == ctx.end())
            return it;
        if (*it != '}')
            throw std::format_error("Invalid format args for PinState");
        return it;
    }

    template<class FmtContext>
    FmtContext::iterator format(Simul::PinState state, FmtContext &ctx) const
    {
        std::ostringstream out;
        out << state;
        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};
