/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <chrono>
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
    bool                   driving;
    PinState               state { PinState::Z };
    std::optional<Handler> on_change {};
    std::optional<Handler> on_update {};
    std::optional<Handler> on_drive {};
    Pin                   *feed { nullptr };
    Pin                   *drive { nullptr };
    bool                   new_driving { false };
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

template<size_t Bits, typename T = uint8_t>
T get_pins(std::array<Pin *, Bits> pins)
{
    T ret { 0 };
    for (int ix = Bits - 1; ix >= 0; --ix) {
        if (pins[ix]->new_state == PinState::Z) {
            return ~static_cast<T>(0);
        }
        ret = (ret << 1) | ((pins[ix]->new_state == PinState::High) ? 0x01 : 0x00);
    }
    return ret;
}

template<size_t N, size_t S1 = N, size_t S2 = N, size_t O1 = 0, size_t O2 = 0>
void assign_pins(std::array<Pin *, S1> const &x, std::array<Pin *, S2> &y)
{
    static_assert(S1 >= O1 + N && S2 >= O2 + N);
    for (auto ix = 0; ix < N; ++ix) {
        y[O2 + ix] = x[O1 + ix];
    }
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
