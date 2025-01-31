/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "Graphics.h"

namespace Simul {

void AbstractPackage::draw_text(float x, float y, std::string const &text)
{
    DrawTextPro(board->font, text.c_str(), Vector2 { x, y }, { 0, 0 }, 0.0f, 15, 2, BLACK);
}

Vector2 AbstractPackage::measure_text(std::string const &text) const
{
    return MeasureTextEx(board->font, text.c_str(), 15, 2);
}

}
