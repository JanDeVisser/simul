/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "GP_Register.h"
#include "Circuit.h"
#include "Graphics.h"
#include "LS04.h"
#include "LS08.h"
#include "LS138.h"
#include "LS245.h"
#include "LS32.h"
#include "LS377.h"
#include "LS574.h"
#include "UtilityDevice.h"

namespace Simul {

ControlBus::ControlBus(Vector2 pin1, Board *board)
    : TriStateSwitch<40, Orientation::North>(Vector2Add(pin1, { 8, 0 }))
{
    for (auto pin = 0; pin < 40; ++pin) {
        tiedowns[pin] = board->circuit.add_component<TieDown>();
        pins[pin] = tiedowns[pin]->Y;
        pins[pin]->state = PinState::Z;
    }
    CLK = pins[2];
    board->add_text(Vector2Add(pin1, { 0, 4 }), "CLK");
    XDATA_ = pins[7];
    board->add_text(Vector2Add(pin1, { 0, 14 }), "XDATA_");
    XADDR_ = pins[8];
    board->add_text(Vector2Add(pin1, { 0, 16 }), "XADDR_");
    IO_ = pins[15];
    board->add_text(Vector2Add(pin1, { 0, 30 }), "IO_");
    for (auto pin = 10; pin < 14; ++pin) {
        OP[pin - 10] = pins[pin];
    }
    board->add_text(Vector2Add(pin1, { 0, 20 }), "OP0");
    for (auto pin = 16; pin < 20; ++pin) {
        PUT[pin - 16] = pins[pin];
    }
    board->add_text(Vector2Add(pin1, { 0, 32 }), "PUT0");
    for (auto pin = 20; pin < 24; ++pin) {
        GET[pin - 20] = pins[pin];
    }
    board->add_text(Vector2Add(pin1, { 0, 40 }), "GET0");
    for (auto pin = 24; pin < 32; ++pin) {
        D[pin - 24] = pins[pin];
    }
    for (auto pin = 32; pin < 40; ++pin) {
        ADDR[pin - 32] = pins[pin];
    }
    for (auto bit = 0; bit < 8; ++bit) {
        board->add_text(Vector2Add(pin1, { 0, 2.0f * (24.0f + bit) }), std::format("D{}", bit));
        board->add_text(Vector2Add(pin1, { 0, 2.0f * (32.0f + bit) }), std::format("A{}", bit));
    }
}

void ControlBus::op_label(int op, std::string const &label)
{
    board->add_text(Vector2Add(pin1, { 7, 20.0f + 2.0f * op }), label);
}

template<size_t Bits>
void set_pins(std::array<Pin *, Bits> pins, uint8_t value)
{
    for (auto ix = 0; ix < Bits; ++ix) {
        pins[ix]->state = (value & 0x01) ? PinState::High : PinState::Low;
        value >>= 1;
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

void EightBit_GP_Register(Board &board)
{
    Pin *PUT_;
    Pin *GET_;
    Pin *IOIn;
    Pin *IOOut;
    Pin *In_;
    Pin *Out_;

    auto *bus = board.add_package<ControlBus>(Vector2 { 1, 1 }, &board);
    bus->op_label(0, "IOin");
    bus->op_label(3, "IOout");

    auto *U1 = board.add_device<LS138, DIP<16, Orientation::North>>(20, 32, "74LS138", "U1");
    auto *U2 = board.add_device<LS138, DIP<16, Orientation::North>>(20, 42, "74LS138", "U2");
    auto *U3 = board.add_device<LS245, DIP<20, Orientation::North>>(30, 42, "74LS245", "U3");
    auto *U4 = board.add_device<LS377, DIP<20, Orientation::North>>(45, 42, "74LS377", "U4");
    auto *U5 = board.add_device<LS04, DIP<14, Orientation::North>>(30, 32, "74LS04", "U5");
    auto *U6 = board.add_device<LS32, DIP<14, Orientation::North>>(37, 32, "74LS32", "U6");
    auto *U7 = board.add_device<LS08, DIP<14, Orientation::North>>(44, 32, "74LS08", "U7");
    auto *U8 = board.add_device<LS32, DIP<14, Orientation::North>>(51, 32, "74LS32", "U8");

    IOIn = bus->OP[0];
    IOOut = bus->OP[3];
    PUT_ = U7->Y[1];
    GET_ = U6->Y[2];
    In_ = U6->Y[0];
    Out_ = U6->Y[1];

    U1->A->feed = bus->PUT[0];
    U1->B->feed = bus->PUT[1];
    U1->C->feed = bus->PUT[2];
    U1->G1->feed = board.circuit.VCC;
    U1->G2A->feed = bus->XDATA_;
    U1->G2B->feed = bus->PUT[3];

    U2->A->feed = bus->GET[0];
    U2->B->feed = bus->GET[1];
    U2->C->feed = bus->GET[2];
    U2->G1->feed = board.circuit.VCC;
    U2->G2A->feed = board.circuit.GND;
    U2->G2B->feed = bus->GET[3];

    U3->DIR->feed = board.circuit.GND;
    U3->OE_->feed = GET_;
    for (auto bit = 0; bit < 8; ++bit) {
        bus->D[bit]->feed = U3->A[bit];
        U3->B[bit]->feed = U4->Q[bit];
    }

    U4->CLK->feed = bus->CLK;
    U4->E_->feed = PUT_;
    for (auto bit = 0; bit < 8; ++bit) {
        U4->D[bit]->feed = bus->D[bit];
    }

    U5->A[0]->feed = IOIn;
    U5->A[1]->feed = IOOut;

    U6->A[0]->feed = U5->Y[0];
    U6->B[0]->feed = bus->IO_;
    U6->A[1]->feed = U5->Y[1];
    U6->B[1]->feed = bus->IO_;
    U6->A[2]->feed = U2->Y[0];
    U6->B[2]->feed = U7->Y[0];
    U6->A[3]->feed = In_;
    U6->B[3]->feed = U2->Y[0];

    U7->A[0]->feed = bus->XDATA_;
    U7->B[0]->feed = Out_;
    U7->A[1]->feed = U8->Y[0];
    U7->B[1]->feed = U6->Y[3];

    U8->A[0]->feed = U1->Y[0];
    U8->B[0]->feed = bus->XDATA_;

    auto signals = board.add_package<LEDArray<4, Orientation::North>>(Vector2 { 57, 1 });
    connect(std::array<Pin *, 4> { PUT_, GET_, IOIn, IOOut }, signals);
    board.add_text({ 52, 1 }, "PUT_");
    board.add_text({ 52, 3 }, "GET_");
    board.add_text({ 52, 5 }, "IOIn");
    board.add_text({ 52, 7 }, "IOOut");

    auto txbus = board.add_package<LEDArray<8, Orientation::North>>(Vector2 { 57, 10 });
    connect(U4->Q, txbus);
    for (auto bit = 0; bit < 8; ++bit) {
        board.add_text({ 54, 10.0f + 2*bit }, std::format("Q{}", bit));
    }

    bus->CLK->state = PinState::Low;
    bus->XDATA_->state = PinState::Low;
    bus->IO_->state = PinState::High;
    bus->set_op(0x00);
    bus->set_put(0x00);
    bus->set_get(0x01);
    bus->set_data(0xAA);
    bus->set_addr(0);
}

}
