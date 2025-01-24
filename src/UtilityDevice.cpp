/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "UtilityDevice.h"

namespace Simul {

TieDown::TieDown(PinState state, std::string const &ref)
    : Device("TieDown", ref)
{
    Y = add_pin(1, "Y");
    Y->state = state;
}

bool TieDown::on() const
{
    return Y->on();
}

}
