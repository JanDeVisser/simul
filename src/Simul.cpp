/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <thread>
#include <vector>

#include <raylib.h>

#include "Graphics.h"
#include "LS04.h"
#include "LS139.h"
#include "Oscillator.h"

namespace Simul {

void main()
{

    //        SetTraceLogLevel(LOG_FATAL);
    InitWindow(30 * static_cast<int>(PITCH), 30 * static_cast<int>(PITCH), "Simul");
    SetWindowState(FLAG_VSYNC_HINT);
    //    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    //    MaximizeWindow();

    Board board;
    oscillator_test(board);
    auto        time = std::chrono::high_resolution_clock::now();
    auto        quit { false };
    std::thread t { [&quit](Board *board) {
                       auto start = std::chrono::high_resolution_clock::now();
                       auto current = start;
                       do {
                           board->circuit.simulate_step(current - start);
                           std::this_thread::sleep_for(1ms);
                           current = std::chrono::high_resolution_clock::now();
                       } while (!quit);
                   },
        &board };

    SetWindowSize(static_cast<int>(board.size.x), static_cast<int>(board.size.y));
    while (!WindowShouldClose()) {
        board.handle_input();
        BeginDrawing();
        board.render();
        EndDrawing();
    }
    quit = true;
    t.join();
    CloseWindow();
}

}

int main()
{
    Simul::main();
    return 0;
}
