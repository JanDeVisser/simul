/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "GP_Register.h"
#include "ControlBus.h"
#include "IC/LS04.h"
#include "IC/LS08.h"
#include "IC/LS138.h"
#include "IC/LS245.h"
#include "IC/LS32.h"
#include "IC/LS377.h"
#include "System.h"

namespace Simul {

GP_Register::GP_Register(System &system, int reg_no)
    : Device(std::format("GP{}", reg_no))
    , bus(system.bus)
    , reg_no(reg_no)
{
    U1 = add_component<LS138>();
    U2 = add_component<LS138>();
    U3 = add_component<LS245>();
    U4 = add_component<LS377>();
    U5 = add_component<LS04>();
    U6 = add_component<LS32>();
    U7 = add_component<LS08>();
    U8 = add_component<LS32>();

    IOIn = bus->OP[0];
    IOOut = bus->OP[3];
    PUT_ = U7->Y[1];
    GET_ = U6->Y[2];
    In_ = U6->Y[0];
    Out_ = U6->Y[1];

    U1->A->feed = bus->PUT[0];
    U1->B->feed = bus->PUT[1];
    U1->C->feed = bus->PUT[2];
    U1->G1->feed = VCC;
    U1->G2A->feed = bus->XDATA_;
    U1->G2B->feed = bus->PUT[3];

    U2->A->feed = bus->GET[0];
    U2->B->feed = bus->GET[1];
    U2->C->feed = bus->GET[2];
    U2->G1->feed = VCC;
    U2->G2A->feed = GND;
    U2->G2B->feed = bus->GET[3];

    U3->DIR->feed = GND;
    U3->OE_->feed = GET_;
    for (auto bit = 0; bit < 8; ++bit) {
        U3->A[bit]->drive = bus->D[bit];
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
    U6->A[2]->feed = U2->Y[reg_no];
    U6->B[2]->feed = U7->Y[0];
    U6->A[3]->feed = In_;
    U6->B[3]->feed = U2->Y[0];

    U7->A[0]->feed = bus->XDATA_;
    U7->B[0]->feed = Out_;
    U7->A[1]->feed = U8->Y[0];
    U7->B[1]->feed = U6->Y[3];

    U8->A[0]->feed = U1->Y[reg_no];
    U8->B[0]->feed = bus->XDATA_;
}

Card make_GP_Register(System &system, int reg_no)
{
    auto  board = system.make_board();
    auto *reg_circuit = system.circuit.add_component<GP_Register>(system, reg_no);
    bus_label(*board, 0, "IOin");
    bus_label(*board, 3, "IOout");

    board->add_device<LS138, DIP<16, Orientation::North>>(reg_circuit->U1, 10, 26, "74LS138", "U1");
    board->add_device<LS138, DIP<16, Orientation::North>>(reg_circuit->U2, 10, 44, "74LS138", "U2");
    board->add_device<LS245, DIP<20, Orientation::North>>(reg_circuit->U3, 38, 35, "74LS245", "U3");
    board->add_device<LS377, DIP<20, Orientation::North>>(reg_circuit->U4, 25, 35, "74LS377", "U4");
    board->add_device<LS04, DIP<14, Orientation::North>>(reg_circuit->U5, 10, 3, "74LS04", "U5");
    board->add_device<LS32, DIP<14, Orientation::North>>(reg_circuit->U6, 20, 3, "74LS32", "U6");
    board->add_device<LS08, DIP<14, Orientation::North>>(reg_circuit->U7, 30, 3, "74LS08", "U7");
    board->add_device<LS32, DIP<14, Orientation::North>>(reg_circuit->U8, 40, 3, "74LS32", "U8");

    auto edge = system.make_board();
    auto signals = edge->add_package<LEDArray<4, Orientation::North>>(6, 1);
    connect(std::array<Pin *, 4> { reg_circuit->PUT_, reg_circuit->GET_, reg_circuit->IOIn, reg_circuit->IOOut }, signals);
    edge->add_text(1, 1, "PUT_");
    edge->add_text(1, 3, "GET_");
    edge->add_text(1, 5, "IOIn");
    edge->add_text(1, 7, "IOOut");

    auto txbus = edge->add_package<LEDArray<8, Orientation::North>>(6, 10);
    connect(reg_circuit->U4->Q, txbus);
    for (auto bit = 0; bit < 8; ++bit) {
        edge->add_text(3, 10 + 2 * bit, std::format("Q{}", bit));
    }
    auto label = std::format("GP {:c}", static_cast<char>(reg_no) + 'A');
    board->add_text(2, 80, label);
    edge->add_text(2, 80, label);
    return { std::move(board), std::move(edge), reg_circuit };
}

}
