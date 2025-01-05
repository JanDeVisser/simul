/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Graphics.h"

namespace Simul {

struct Card {
    std::unique_ptr<Board> board;
    std::unique_ptr<Board> edge;
};

struct System {
    Circuit                circuit;
    struct ControlBus     *bus;
    int                    current_card { 0 };
    std::unique_ptr<Board> backplane;
    std::vector<Card>      cards;
    Font                   font;
    Vector2                size {};

    explicit System(Font font);
    std::unique_ptr<Board> make_board();
    void                   layout();
    void                   handle_input();
    void                   render();
};

}
