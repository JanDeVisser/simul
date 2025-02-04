/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <thread>
#include <vector>

#include <raylib.h>

#include "Circuit/Graphics.h"
#include "MicroCode.h"
#include "System.h"

namespace Simul {

void main(int argc, char **argv)
{
    InitWindow(30 * static_cast<int>(PITCH), 30 * static_cast<int>(PITCH), "Simul");
    SetWindowState(FLAG_VSYNC_HINT);
    {
        auto   font = LoadFontEx("fonts/Tecnico-Bold.ttf", 15, nullptr, 0);
        System system(font);
        if (argc > 1) {
            if (auto mc_maybe = parse_microcode(argv[1]); mc_maybe.is_error()) {
                std::cerr << mc_maybe.error() << "\n";
                exit(1);
            } else {
                std::swap(mc_maybe.value(), system.microcode);
            }
        }
        auto t = system.simulate();
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

int main(int argc, char **argv)
{
    Simul::main(argc, argv);
    return 0;
}
