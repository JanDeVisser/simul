/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <App/MicroCode.h>
#include <App/Monitor.h>
#include <Circuit/Graphics.h>
#include <Circuit/Memory.h>
#include <Circuit/Oscillator.h>

namespace Simul {

struct Card {
    std::unique_ptr<Board> board;
    std::unique_ptr<Board> edge;
    Device                *circuit {};
};

struct System {
    Circuit                   &circuit;
    struct ControlBus         *bus;
    std::optional<int>         current_card {};
    std::unique_ptr<Board>     backplane;
    std::vector<Card>          cards;
    Font                       font;
    Vector2                    size {};
    std::vector<MicroCodeStep> microcode {};
    size_t                     current_step { 0 };
    EEPROM_28C256             *rom;
    SRAM_LY62256              *ram;
    struct Monitor            *monitor;

    explicit System(Font font);
    std::unique_ptr<Board> make_board();
    std::thread            simulate();
    void                   layout();
    void                   handle_input();
    void                   render();
};

}
