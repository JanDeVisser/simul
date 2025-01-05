/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "ControlBus.h"
#include "Graphics.h"

namespace Simul {

ControlBus::ControlBus()
    : Device("BUS")
{
    for (auto pin = 0; pin < 40; ++pin) {
        tiedowns[pin] = add_component<TieDown>(PinState::Z);
    }
    CLK = tiedowns[2]->Y;
    XDATA_ = tiedowns[7]->Y;
    XADDR_ = tiedowns[8]->Y;
    IO_ = tiedowns[15]->Y;
    for (auto pin = 10; pin < 14; ++pin) {
        OP[pin - 10] = tiedowns[pin]->Y;
    }
    for (auto pin = 16; pin < 20; ++pin) {
        PUT[pin - 16] = tiedowns[pin]->Y;
    }
    for (auto pin = 20; pin < 24; ++pin) {
        GET[pin - 20] = tiedowns[pin]->Y;
    }
    for (auto pin = 24; pin < 32; ++pin) {
        D[pin - 24] = tiedowns[pin]->Y;
    }
    for (auto pin = 32; pin < 40; ++pin) {
        ADDR[pin - 32] = tiedowns[pin]->Y;
    }
}

void ControlBus::set_op(uint8_t op)
{
    set_pins(OP, op);
}

void ControlBus::set_put(uint8_t op)
{
    set_pins(PUT, op);
}

void ControlBus::set_get(uint8_t op)
{
    set_pins(GET, op);
}

void ControlBus::set_data(uint8_t op)
{
    set_pins(D, op);
}

void ControlBus::set_addr(uint8_t op)
{
    set_pins(ADDR, op);
}

void bus_label(Board &board, int op, std::string const &label)
{
    board.add_text(1, 21 + 2 * op, label);
}

template<Orientation O>
void connect(ControlBus *device, TriStateSwitch<40, O> *package)
{
    std::array<Pin *, 40> a {};
    for (auto ix = 0; ix < 40; ++ix) {
        a[ix] = device->tiedowns[ix]->Y;
    }
    connect(a, package);
}

ControlBus *make_backplane(System &system)
{
    system.backplane = system.make_board();
    auto &board = *system.backplane;
    auto  bus = board.add_device<ControlBus, TriStateSwitch<40, Orientation::North>>(9, 1);
    board.add_text(1, 5, "CLK");
    board.add_text(1, 15, "XDATA_");
    board.add_text(1, 17, "XADDR_");
    board.add_text(1, 31, "IO_");
    board.add_text(1, 21, "OP0");
    board.add_text(1, 33, "PUT0");
    board.add_text(1, 41, "GET0");
    for (auto bit = 0; bit < 8; ++bit) {
        board.add_text(4, 2 * (25 + bit) - 1, std::format("D{}", bit));
        board.add_text(4, 2 * (33 + bit) - 1, std::format("A{}", bit));
    }
    return bus;
}

}
