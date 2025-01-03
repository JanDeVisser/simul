/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>
#include <vector>

#include <raylib.h>
#include <raymath.h>

#include <Circuit.h>
#include <Device.h>
#include <Pin.h>

namespace Simul {

enum class Orientation {
    North = -90,
    West = 0,
    South = 90,
    East = 180,
};

static constexpr float PITCH = (4 * 2.54f);

struct AbstractPackage {
    Device       *device {};
    struct Board *board {};

    Rectangle rect {};
    virtual ~AbstractPackage() = default;
    virtual void render() = 0;
    virtual void handle_input()
    {
    }
};

template<size_t S>
struct Package : public AbstractPackage {
    std::array<Pin *, S> pins {};
    Vector2              pin1 {};
    Vector2              pin1_tx {};

    explicit Package(Vector2 pin1)
        : pin1(pin1)
        , pin1_tx(Vector2Scale(pin1, PITCH))
    {
    }
};

inline Color pin_color(Pin *pin)
{
    switch (pin->state) {
    case PinState::Z:
        return DARKGRAY;
    case PinState::Low:
        return DARKPURPLE;
    case PinState::High:
        return RED;
    }
}

template<size_t S, Orientation O = Orientation::West>
struct LEDArray : public Package<S> {
    Vector2 incr {};

    explicit LEDArray(Vector2 pin1)
        : Package<S>(pin1)
    {
        switch (O) {
        case Orientation::West:
            AbstractPackage::rect = Rectangle { PITCH * pin1.x, PITCH * (pin1.y - 2), PITCH * S * 2, PITCH * 2 };
            incr = { 2.0f * PITCH, 0 };
            break;
        case Orientation::East:
            AbstractPackage::rect = Rectangle { PITCH * (pin1.x - S), PITCH * pin1.y, PITCH * S * 2, PITCH * 2 };
            incr = { -2.0f * PITCH, 0 };
            break;
        case Orientation::North:
            AbstractPackage::rect = Rectangle { PITCH * pin1.x, PITCH * pin1.y, PITCH * 2, PITCH * S * 2 };
            incr = { 0, 2.0f * PITCH };
            break;
        case Orientation::South:
            AbstractPackage::rect = Rectangle { PITCH * (pin1.x - 2), PITCH * (pin1.y - S), PITCH * 2, PITCH * S * 2 };
            incr = { 0, -2.0f * PITCH };
            break;
        }
    }

    void render() override
    {
        Vector2 p { Package<S>::pin1_tx };
        DrawRectangleRounded(AbstractPackage::rect, 0.5, 2, BLACK);
        for (auto ix = 0; ix < S; ++ix) {
            auto color = pin_color(Package<S>::pins[ix]);
            DrawRectangleRounded({ p.x + 2, p.y + 2, PITCH * 2.0f - 4, PITCH * 2.0f - 4 }, 1.0, 2, color);
            p = Vector2Add(p, incr);
        }
    }
};

template<int S, Orientation O = Orientation::West>
struct DIPSwitch : public Package<S> {
    std::array<Pin *, S> pins {};
    Vector2              position {};
    Vector2              incr {};
    Vector2              switch_on {};
    Vector2              switch_off {};
    Vector2              size { 2 * PITCH - 4, 2 * PITCH - 4 };
    Vector2              double_size { 2 * PITCH - 2, 4 * PITCH - 2 };

    explicit DIPSwitch(Vector2 pin1)
        : Package<S>(pin1)
    {
        switch (O) {
        case Orientation::West:
            AbstractPackage::rect = Rectangle { PITCH * pin1.x - 4, PITCH * (pin1.y - 4) - 4, PITCH * S * 2 + 8, PITCH * 4 + 8 };
            position = { PITCH * pin1.x + 2, PITCH * (pin1.y - 4) + 2 };
            incr = Vector2Scale({ 1, 0 }, PITCH);
            switch_on = { 0, 0 };
            switch_off = { 0, 2 * PITCH };
            break;
        case Orientation::East:
            AbstractPackage::rect = Rectangle { PITCH * (pin1.x - S) - 4, PITCH * pin1.y - 4, PITCH * S * 2 + 8, PITCH * 4 + 8 };
            position = { PITCH * (pin1.x - 2 * S) + 2, pin1.y + 2 };
            incr = { -2 * PITCH, 0 };
            switch_on = { 0, 2 * PITCH };
            switch_off = { 0, 0 };
            break;
        case Orientation::North:
            AbstractPackage::rect = Rectangle { PITCH * pin1.x - 4, PITCH * pin1.y - 4, PITCH * 4 + 8, PITCH * S * 2 + 8 };
            position = { PITCH * pin1.x + 2, PITCH * pin1.y + 2 };
            incr = { 0, 2 * PITCH };
            switch_on = { 2 * PITCH, 0 };
            switch_off = { 0, 0 };
            double_size = { 4 * PITCH - 2, 2 * PITCH - 2 };
            break;
        case Orientation::South:
            AbstractPackage::rect = Rectangle { PITCH * (pin1.x - 2) - 4, PITCH * (pin1.y - S) - 4, PITCH * 4 + 8, PITCH * S * 2 + 8 };
            position = { PITCH * (pin1.x - 4) + 2, PITCH * (pin1.y - 2 * S) + 2 };
            incr = { 0, -2 * PITCH };
            switch_on = { 0, 0 };
            switch_off = { 2 * PITCH, 0 };
            double_size = { 4 * PITCH - 2, 2 * PITCH - 2 };
            break;
        }
    }

    void handle_input() override
    {
        if (!IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            return;
        }
        Vector2 p = position;
        for (auto ix = 0; ix < S; ++ix) {
            Rectangle r { p.x - 1, p.y - 1, double_size.x, double_size.y };
            if (CheckCollisionPointRec(GetMousePosition(), r)) {
                pins[ix]->state = !pins[ix]->state;
                break;
            }
            p = Vector2Add(p, incr);
        }
    }

    void render() override
    {
        DrawRectangleRounded(AbstractPackage::rect, 0.3, 10, BLACK);
        Vector2 p = position;
        for (auto ix = 0; ix < S; ++ix) {
            Color color = (pins[ix] && pins[ix]->on()) ? RED : DARKPURPLE;
            DrawRectangleV(Vector2Add(p, (pins[ix] && pins[ix]->on()) ? switch_on : switch_off), size, color);
            Rectangle r { p.x - 1, p.y - 1, double_size.x, double_size.y };
            if (CheckCollisionPointRec(GetMousePosition(), r)) {
                DrawRectangleRoundedLines(r, 0.3, 10, 1, GOLD);
            }
            p = Vector2Add(p, incr);
        }
    }
};

template<int S, Orientation O = Orientation::West>
struct TriStateSwitch : public Package<S> {
    std::array<Pin *, S> pins {};
    Vector2              position {};
    Vector2              incr {};
    Vector2              switch_on {};
    Vector2              switch_z {};
    Vector2              switch_off {};
    Vector2              size { 2 * PITCH - 4, 2 * PITCH - 4 };
    Vector2              full_size { 2 * PITCH - 2, 6 * PITCH - 2 };

    explicit TriStateSwitch(Vector2 pin1)
        : Package<S>(pin1)
    {
        switch (O) {
        case Orientation::West:
            AbstractPackage::rect = Rectangle { PITCH * pin1.x - 4, PITCH * (pin1.y - 4) - 4, PITCH * S * 2 + 8, PITCH * 6 + 8 };
            position = { PITCH * pin1.x + 2, PITCH * (pin1.y - 6) + 2 };
            incr = Vector2Scale({ 1, 0 }, PITCH);
            switch_on = { 0, 0 };
            switch_z = { 0, 2 * PITCH };
            switch_off = { 0, 4 * PITCH };
            break;
        case Orientation::East:
            AbstractPackage::rect = Rectangle { PITCH * (pin1.x - S) - 4, PITCH * pin1.y - 4, PITCH * S * 2 + 8, PITCH * 6 + 8 };
            position = { PITCH * (pin1.x - 2 * S) + 2, pin1.y + 2 };
            incr = { -2 * PITCH, 0 };
            switch_on = { 0, 4 * PITCH };
            switch_z = { 0, 2 * PITCH };
            switch_off = { 0, 0 };
            break;
        case Orientation::North:
            AbstractPackage::rect = Rectangle { PITCH * pin1.x - 4, PITCH * pin1.y - 4, PITCH * 6 + 8, PITCH * S * 2 + 8 };
            position = { PITCH * pin1.x + 2, PITCH * pin1.y + 2 };
            incr = { 0, 2 * PITCH };
            switch_on = { 4 * PITCH, 0 };
            switch_z = { 2 * PITCH, 0 };
            switch_off = { 0, 0 };
            full_size = { 6 * PITCH - 2, 2 * PITCH - 2 };
            break;
        case Orientation::South:
            AbstractPackage::rect = Rectangle { PITCH * (pin1.x - 2) - 4, PITCH * (pin1.y - S) - 4, PITCH * 6 + 8, PITCH * S * 2 + 8 };
            position = { PITCH * (pin1.x - 6) + 2, PITCH * (pin1.y - 2 * S) + 2 };
            incr = { 0, -2 * PITCH };
            switch_on = { 0, 0 };
            switch_z = { 2 * PITCH, 0 };
            switch_off = { 4 * PITCH, 0 };
            full_size = { 6 * PITCH - 2, 2 * PITCH - 2 };
            break;
        }
    }

    void handle_input() override
    {
        if (!IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            return;
        }
        Vector2 p = position;
        for (auto ix = 0; ix < S; ++ix) {
            {
                Rectangle r { p.x - 1 + switch_on.x, p.y - 1 + switch_on.y, size.x, size.y };
                if (CheckCollisionPointRec(GetMousePosition(), r)) {
                    pins[ix]->state = PinState::High;
                }
            }
            {
                Rectangle r { p.x - 1 + switch_off.x, p.y - 1 + switch_off.y, size.x, size.y };
                if (CheckCollisionPointRec(GetMousePosition(), r)) {
                    pins[ix]->state = PinState::Low;
                }
            }
            {
                Rectangle r { p.x - 1 + switch_z.x, p.y - 1 + switch_z.y, size.x, size.y };
                if (CheckCollisionPointRec(GetMousePosition(), r)) {
                    pins[ix]->state = PinState::Z;
                }
            }
            p = Vector2Add(p, incr);
        }
    }

    void render() override
    {
        DrawRectangleRounded(AbstractPackage::rect, 0.3, 10, BLACK);
        Vector2 p = position;
        for (auto ix = 0; ix < S; ++ix) {
            Color   color = pin_color(pins[ix]);
            Vector2 offset;
            switch (pins[ix]->state) {
            case PinState::Low:
                offset = switch_off;
                break;
            case PinState::Z:
                offset = switch_z;
                break;
            case PinState::High:
                offset = switch_on;
                break;
            }
            DrawRectangleV(Vector2Add(p, offset), size, color);
            Rectangle r { p.x - 1, p.y - 1, full_size.x, full_size.y };
            if (CheckCollisionPointRec(GetMousePosition(), r)) {
                DrawRectangleRoundedLines(r, 0.3, 10, 1, GOLD);
            }
            p = Vector2Add(p, incr);
        }
    }
};

template<size_t S, Orientation O = Orientation::West>
struct DIP : public Package<S> {
    std::array<Pin *, S> pins {};
    Vector2              first_row {};
    Vector2              row_offset {};
    Vector2              second_row {};

    explicit DIP(Vector2 pin1)
        : Package<S>(pin1)
    {
        switch (O) {
        case Orientation::West:
            AbstractPackage::rect = Rectangle { PITCH * (pin1.x - 1), PITCH * (pin1.y - 3), PITCH * (S / 2.0f + 1), PITCH * 3 };
            first_row = Vector2Scale({ 1, 0 }, PITCH);
            row_offset = Vector2Scale({ 0, -3 }, PITCH);
            second_row = Vector2Scale({ -1, 0 }, PITCH);
            break;
        case Orientation::East:
            AbstractPackage::rect = Rectangle { PITCH * (pin1.x - (S / 2.0f)), pin1.y, PITCH * (S / 2.0f + 1), PITCH * 3 };
            first_row = Vector2Scale({ -1, 0 }, PITCH);
            row_offset = Vector2Scale({ 0, 3 }, PITCH);
            second_row = Vector2Scale({ 1, 0 }, PITCH);
            break;
        case Orientation::North:
            AbstractPackage::rect = Rectangle { PITCH * pin1.x, PITCH * (pin1.y - 1), PITCH * 3, PITCH * (S / 2.0f + 1) };
            first_row = Vector2Scale({ 0, 1 }, PITCH);
            row_offset = Vector2Scale({ 3, 0 }, PITCH);
            second_row = Vector2Scale({ 0, -1 }, PITCH);
            break;
        case Orientation::South:
            AbstractPackage::rect = Rectangle { PITCH * (pin1.x - 3), PITCH * (pin1.y - (S / 2.0f)), PITCH * 3, PITCH * (S / 2.0f + 1) };
            first_row = Vector2Scale({ 0, -1 }, PITCH);
            row_offset = Vector2Scale({ -3, 0 }, PITCH);
            second_row = Vector2Scale({ 0, 1 }, PITCH);
            break;
        }
    }

    void render() override
    {
        Vector2 p { Package<S>::pin1_tx };
        DrawRectangleLinesEx(AbstractPackage::rect, 1, BLACK);
        for (auto ix = 0; ix < S / 2; ++ix) {
            Color color = (pins[ix] && pins[ix]->on()) ? RED : DARKPURPLE;
            DrawCircleV(p, PITCH / 2, color);
            if (ix < S / 2 - 1) {
                p = Vector2Add(p, first_row);
            }
        }
        p = Vector2Add(p, row_offset);
        for (auto ix = S / 2; ix < S; ++ix) {
            Color color = (pins[ix] && pins[ix]->on()) ? RED : DARKPURPLE;
            DrawCircleV(p, PITCH / 2, color);
            p = Vector2Add(p, second_row);
        }
    }
};

template<typename D, typename P>
inline void connect(D const &device, P package)
{
    static_assert(false);
}

template<typename D, typename P>
inline void connect(D *device, P package)
{
    static_assert(false);
}

template<typename P, size_t S>
    requires std::derived_from<P, Package<S>>
inline void connect(std::array<Pin *, S> device, P *package)
{
    for (auto ix = 0; ix < S; ++ix) {
        package->pins[ix] = device[ix];
    }
}

template<typename P>
    requires std::derived_from<P, Package<1>>
inline void connect(Pin *device, P *package)
{
    package->pins[0] = device;
}

struct Board {
    struct Text {
        Vector2     pos;
        std::string text;
        float       angle { 0.0 };
    };

    Font                                          font;
    Vector2                                       size {};
    std::vector<std::unique_ptr<AbstractPackage>> packages {};
    Circuit                                       circuit {};
    std::vector<Text>                             texts;

    Board()
        : font(GetFontDefault())
    {
    }

    explicit Board(Font font)
        : font(font)
    {
    }

    explicit Board(std::string const &name)
        : Board()
    {
        circuit = std::move(Circuit { name });
    }

    void render()
    {
        ClearBackground(DARKGREEN);
        for (auto const &p : packages) {
            p->render();
        }
        for (auto const &text : texts) {
            auto p = Vector2Scale(text.pos, PITCH);
            DrawTextPro(font, text.text.data(), p, { 0, 0 }, text.angle, 20, 2, BLACK);
        }
    }

    void handle_input()
    {
        for (auto const &p : packages) {
            if (CheckCollisionPointRec(GetMousePosition(), p->rect)) {
                p->handle_input();
                break;
            }
        }
    }

    void add_text(Vector2 pos, std::string text, float angle = 0.0f)
    {
        texts.emplace_back(pos, std::move(text), angle);
    }

    template<class P, typename... Args>
    P *add_package(Args &&...args)
    {
        auto *ptr = new P(args...);
        size.x = std::max(size.x, ptr->rect.x + ptr->rect.width + PITCH);
        size.y = std::max(size.y, ptr->rect.y + ptr->rect.height + PITCH);
        packages.emplace_back(dynamic_cast<AbstractPackage *>(ptr));
        ptr->board = this;
        return ptr;
    }

    template<class D, class P, typename... Args>
    P *add_device(D *device, Args &&...args)
    {
        auto *ptr = add_package<P>(args...);
        ptr->device = device;
        connect(device, ptr);
        return ptr;
    }

    template<class D, class P, typename... Args>
    D *add_device(int px, int py, std::string const &name = "", std::string const &ref = "", Args &&...args)
    {
        auto *device = circuit.add_component<D>(args...);
        auto *pkg = add_device<D, P>(device, Vector2 { static_cast<float>(px), static_cast<float>(py) });
        if (!ref.empty()) {
            add_text(Vector2Add(pkg->pin1, { 4, -2 }), ref);
        }
        if (!name.empty()) {
            add_text(Vector2Add(pkg->pin1, { 2, 0 }), name, 90);
        }
        return device;
    }
};

}
