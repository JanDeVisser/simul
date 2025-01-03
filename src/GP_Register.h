/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Graphics.h"
#include "UtilityDevice.h"

namespace Simul {

struct ControlBus : public TriStateSwitch<40, Orientation::North> {
    Pin                      *CLK;
    Pin                      *XDATA_;
    Pin                      *XADDR_;
    Pin                      *IO_;
    std::array<Pin *, 4>      OP {};
    std::array<Pin *, 4>      PUT {};
    std::array<Pin *, 4>      GET {};
    std::array<Pin *, 8>      D {};
    std::array<Pin *, 8>      ADDR {};
    std::array<TieDown *, 40> tiedowns {};

    ControlBus(Vector2 pin1, Board *board);
    void op_label(int op, std::string const& label);
    void set_op(uint8_t op);
    void set_put(uint8_t op);
    void set_get(uint8_t op);
    void set_data(uint8_t op);
    void set_addr(uint8_t op);
};

void EightBit_GP_Register(Board &);

}
