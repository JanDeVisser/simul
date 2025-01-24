/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "Monitor.h"
#include "ControlBus.h"
#include "IC/LS04.h"
#include "IC/LS08.h"
#include "IC/LS138.h"
#include "IC/LS245.h"
#include "IC/LS32.h"
#include "IC/LS377.h"
#include "System.h"

namespace Simul {

Monitor::Monitor(System &system)
    : Device("Mon")
    , bus(system.bus)
{
    U1 = add_component<LS138>();
    U3 = add_component<LS245>();
    U4 = add_component<LS245>();
    U6 = add_component<LS32>();
    U7 = add_component<LS08>();

    for (auto ix = 0; ix < 8; ++ix) {
        SW1[ix] = add_pin(ix, std::format("SW1{}", ix), PinState::Low);
    }
    for (auto ix = 0; ix < 8; ++ix) {
        SW2[ix] = add_pin(ix, std::format("SW2{}", ix), PinState::Low);
    }

    GET_ = U1->Y[7];

    U1->A->feed = bus->GET[1];
    U1->B->feed = bus->GET[2];
    U1->C->feed = bus->GET[3];
    U1->G1->feed = Circuit::the().VCC;
    U1->G2A->feed = U7->Y[0];
    U1->G2B->feed = bus->GET[0];

    U3->DIR->feed = Circuit::the().GND;
    U3->OE_->feed = GET_;
    for (auto bit = 0; bit < 8; ++bit) {
        U3->A[bit]->drive = bus->D[bit];
        U3->B[bit]->feed = SW1[bit];
    }

    U4->DIR->feed = Circuit::the().GND;
    U4->OE_->feed = U6->Y[0];
    for (auto bit = 0; bit < 8; ++bit) {
        U4->A[bit]->drive = bus->ADDR[bit];
        U4->B[bit]->feed = SW2[bit];
    }

    U6->A[0]->feed = bus->XADDR_;
    U6->B[0]->feed = GET_;

    U7->A[0]->feed = bus->XDATA_;
    U7->B[0]->feed = bus->XADDR_;
}

Card make_Monitor(System &system)
{
    auto  board = system.make_board();
    auto *monitor_circuit = system.circuit.add_component<Monitor>(system);

    board->add_device<LS138, DIP<16, Orientation::North>>(monitor_circuit->U1, 10, 26, "74LS138", "U1");
    board->add_device<LS245, DIP<20, Orientation::North>>(monitor_circuit->U3, 26, 26, "74LS245", "U3");
    board->add_device<LS245, DIP<20, Orientation::North>>(monitor_circuit->U4, 36, 26, "74LS245", "U4");
    board->add_device<LS32, DIP<14, Orientation::North>>(monitor_circuit->U6, 20, 3, "74LS32", "U6");
    board->add_device<LS08, DIP<14, Orientation::North>>(monitor_circuit->U7, 30, 3, "74LS08", "U7");

    auto edge = system.make_board();
    auto signals = edge->add_package<LEDArray<1, Orientation::North>>(6, 3);
    connect(monitor_circuit->GET_, signals);
    edge->add_text(1, 3, "GET_");
    signals->on_click[0] = [&system](Pin *) -> void {
        system.bus->data_transfer(0x0E, 0xFF);
    };

    auto d_switches = edge->add_package<DIPSwitch<8, Orientation::North>>(6, 14);
    connect(monitor_circuit->SW1, d_switches);
    for (auto bit = 0; bit < 8; ++bit) {
        edge->add_text(3, 14 + 2 * bit, std::format("D{}", bit));
    }
    auto a_switches = edge->add_package<DIPSwitch<8, Orientation::North>>(6, 32);
    connect(monitor_circuit->SW2, a_switches);
    for (auto bit = 0; bit < 8; ++bit) {
        edge->add_text(3, 32 + 2 * bit, std::format("A{}", bit));
    }
    return { std::move(board), std::move(edge), monitor_circuit };
}

}
