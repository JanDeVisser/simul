/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Graphics.h"
#include "UtilityDevice.h"

namespace Simul {

struct PushButton : public Package<1> {
    Vector2                                   size { 4 * PITCH, 4 * PITCH };
    std::optional<std::function<void(Pin *)>> on_click {};

    explicit PushButton(Vector2 pin1)
        : Package<1>(pin1)
    {
        rect = Rectangle { PITCH * pin1.x, PITCH * pin1.y, PITCH * 4, PITCH * 4 };
    }

    void handle_input() override
    {
        if (!IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            return;
        }
        if (CheckCollisionPointRec(GetMousePosition(), rect)) {
            if (on_click) {
                (*on_click)(pins[0]);
            } else {
                pins[0]->flip();
            }
        }
    }

    void render() override
    {
        DrawRectangleRounded(rect, 0.3, 10, BLACK);
        Color color = pin_color(pins[0]);
        auto  r = rect;
        r.x += 2;
        r.y += 2;
        r.width -= 4;
        r.height -= 4;
        if (CheckCollisionPointRec(GetMousePosition(), r)) {
            DrawRectangleRoundedLines(r, 0.3, 10, GOLD);
        }
        r.x += 2;
        r.y += 2;
        r.width -= 4;
        r.height -= 4;
        DrawRectangleRec(r, color);
    }
};

}
