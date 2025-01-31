/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <thread>
#include <vector>

#include <raylib.h>

#include "Circuit/Graphics.h"
#include "System.h"

namespace Simul {

void main()
{
    InitWindow(30 * static_cast<int>(PITCH), 30 * static_cast<int>(PITCH), "Simul");
    SetWindowState(FLAG_VSYNC_HINT);
    {
        auto   font = LoadFontEx("fonts/Tecnico-Bold.ttf", 15, nullptr, 0);
        System system(font);
        auto   t = system.circuit.start_simulation();
        SetTargetFPS(60);
        {
            SetWindowSize(static_cast<int>(system.size.x), static_cast<int>(system.size.y));
            while (!WindowShouldClose()) {
                system.handle_input();
                BeginDrawing();
                system.render();
                EndDrawing();
            }
            system.circuit.stop();
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
