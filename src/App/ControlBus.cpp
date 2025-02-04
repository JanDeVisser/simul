/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <App/ControlBus.h>
#include <App/Monitor.h>
#include <Circuit/Graphics.h>
#include <Circuit/Oscillator.h>
#include <Circuit/PushButton.h>

namespace Simul {

ControlBus::ControlBus()
    : Device("BUS")
{
    clock_switch = add_component<Switch<200>>();
    oscillator = add_component<Oscillator>(1);
    for (auto pin = 0; pin < 40; ++pin) {
        tiedowns[pin] = add_component<TieDown>(PinState::Low);
    }
    GND = tiedowns[0]->Y;
    GND->state = PinState::Low;
    VCC = tiedowns[1]->Y;
    VCC->state = PinState::High;
    CLK = tiedowns[2]->Y;
    CLK->feed = clock_switch->Y;
    CLK_ = tiedowns[3]->Y;
    invert(CLK, CLK_);
    CLKburst = tiedowns[4]->Y;
    auto *burst = add_component<BurstTrigger>(100ms);
    burst->A->feed = CLK;
    CLKburst->feed = burst->Y;
    HLT_ = tiedowns[5]->Y;
    HLT_->state = PinState::High;
    SUS_ = tiedowns[6]->Y;
    SUS_->state = PinState::High;
    XDATA_ = tiedowns[7]->Y;
    XDATA_->state = PinState::High;
    XADDR_ = tiedowns[8]->Y;
    XADDR_->state = PinState::High;
    SACK_ = tiedowns[9]->Y;
    SACK_->state = PinState::High;
    RST = tiedowns[14]->Y;
    IO_ = tiedowns[15]->Y;
    IO_->state = PinState::High;
    for (auto pin = 0; pin < 24; ++pin) {
        controls[pin] = tiedowns[pin]->Y;
    }
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

void ControlBus::data_transfer(uint8_t from, uint8_t to, uint8_t op) const
{
    XDATA_->new_state = PinState::Low;
    XADDR_->new_state = PinState::High;
    if (from != 0xFF) {
        set_pins(GET, from & 0x0F);
    }
    if (to != 0xFF) {
        set_pins(PUT, to & 0x0F);
    }
    set_pins(OP, op & 0x0F);
}

void ControlBus::addr_transfer(uint8_t from, uint8_t to, uint8_t op) const
{
    XDATA_->new_state = PinState::High;
    XADDR_->new_state = PinState::Low;
    if (from != 0xFF) {
        set_pins(GET, from & 0x0F);
    }
    if (to != 0xFF) {
        set_pins(PUT, to & 0x0F);
    }
    set_pins(OP, op & 0x0F);
}

void ControlBus::enable_oscillator()
{
    CLK->feed = oscillator->Y;
}

void ControlBus::disable_oscillator()
{
    CLK->feed = clock_switch->Y;
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
    auto  bus = system.circuit.add_component<ControlBus>();
    auto  controls = board.add_package<TriStateSwitch<24, Orientation::North>>(9, 1);
    connect(bus->controls, controls);
    for (auto ix = 0; ix < 5; ++ix) {
        controls->disabled[ix] = true;
    }
    auto button = board.add_package<PushButton>(1, 51);
    connect(bus->clock_switch->Y, button);
    auto d_leds = board.add_package<LEDArray<8, Orientation::North>>(11, 51);
    connect(bus->D, d_leds);
    auto a_leds = board.add_package<LEDArray<8, Orientation::North>>(11, 68);
    connect(bus->ADDR, a_leds);
    board.add_text(1, 1, "GND");
    board.add_text(1, 3, "VCC");
    board.add_text(1, 5, "CLK");
    board.add_text(1, 7, "CLK_");
    board.add_text(1, 9, "CLK^");
    board.add_text(1, 11, "HLT_");
    board.add_text(1, 13, "SUS_");
    board.add_text(1, 15, "XDATA_");
    board.add_text(1, 17, "XADDR_");
    board.add_text(1, 29, "RST");
    board.add_text(1, 31, "IO_");
    board.add_text(1, 21, "OP0");
    board.add_text(1, 33, "PUT0");
    board.add_text(1, 41, "GET0");
    for (auto bit = 0; bit < 8; ++bit) {
        board.add_text(8, 2 * (25 + bit) + 1, std::format("D{}", bit));
        board.add_text(8, 2 * (33 + bit) + 2, std::format("A{}", bit));
    }
    return bus;
}

}
