/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <thread>
#include <vector>

#include <raylib.h>

#include "Graphics.h"
#include "GP_Register.h"
#include "LS245.h"
#include "LS377.h"
#include "System.h"

namespace Simul {

void main()
{
    InitWindow(30 * static_cast<int>(PITCH), 30 * static_cast<int>(PITCH), "Simul");
    {
        SetWindowState(FLAG_VSYNC_HINT);
        SetTargetFPS(60);
        auto font = LoadFontEx("fonts/Tecnico-Bold.ttf", 20, nullptr, 0);
        {

            Circuit circuit;
            System system(font) ;
//            LS245_test(board);
            auto        time = std::chrono::high_resolution_clock::now();
            auto        quit { false };
            std::thread t { [&]() {
                               auto start = std::chrono::high_resolution_clock::now();
                               auto current = start;
                               do {
                                   system.circuit.simulate_step(current - start);
                                   std::this_thread::sleep_for(1ms);
                                   current = std::chrono::high_resolution_clock::now();
                               } while (!quit);
                           } };

            SetWindowSize(static_cast<int>(system.size.x), static_cast<int>(system.size.y));
            while (!WindowShouldClose()) {
                system.handle_input();
                BeginDrawing();
                system.render();
                EndDrawing();
            }
            quit = true;
            t.join();
        }
        UnloadFont(font);
    }
    CloseWindow();
}

}

int main()
{
    Simul::main();
    return 0;
}
