/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "Mem_Register.h"

namespace Simul {

Mem_Register::Mem_Register(System &system)
    : Device("Mem")
    , bus(system.bus)
{
    U1 = add_component<LS138>();
    U2 = add_component<LS138>();
    U3 = add_component<LS32>();
    U4 = add_component<LS04>();
    U5 = add_component<LS08>();
    U6 = add_component<LS245>();
    U7 = add_component<LS377>();
    U8 = add_component<LS377>();
    U9 = add_component<SRAM_LY62256>();
    U10 = add_component<EEPROM_28C256>();

    U1->A->feed = bus->GET[0];
    U1->B->feed = bus->GET[1];
    U1->C->feed = bus->GET[2];
    U1->G1->feed = bus->GET[2];
    U1->G2A->feed = bus->XDATA_;
    U1->G2B->feed = bus->GET[3];
    DataGet_ = U1->Y[7];

    U2->A->feed = bus->PUT[1];
    U2->B->feed = bus->PUT[2];
    U2->C->feed = bus->PUT[3];
    U2->G1->feed = bus->PUT[0];
    U2->G2A->feed = Circuit::the().GND;
    U2->G2B->feed = Circuit::the().GND;

    U3->A[0]->feed = bus->XDATA_;
    U3->B[0]->feed = U2->Y[3];
    DataPut_ = U3->Y[0];

    U3->A[1]->feed = bus->XADDR_;
    U3->B[1]->feed = U2->Y[7];
    AddrPut_ = U3->Y[1];

    U5->A[0]->feed = DataGet_;
    U5->B[0]->feed = DataPut_;
    Data_ = U5->Y[0];

    U4->A[0]->feed = bus->CLKburst;
    U3->A[3]->feed = DataPut_;
    U3->B[3]->feed = U4->Y[0];
    DataClk_ = U3->Y[3];

    U6->OE_->feed = Data_;
    U6->DIR->feed = DataGet_;
    connect_pins<8>(bus->D, U6->A);
    drive_pins<8>(U6->A, bus->D);
    drive_pins<8>(U6->B, U9->D);

    U7->E_->feed = AddrPut_;
    U7->CLK->feed = bus->CLK;
    connect_pins<8>(bus->D, U7->D);

    U8->E_->feed = AddrPut_;
    U8->CLK->feed = bus->CLK;
    connect_pins<8>(bus->ADDR, U8->D);
    U4->A[1]->feed = U8->Q[7];

    U9->CE_->feed = U4->Y[1];
    U9->OE_->feed = DataGet_;
    U9->WE_->feed = DataClk_;
    drive_pins<8>(U9->D, U6->B);
    connect_pins<8>(U7->Q, U9->A);
    connect_pins<7, 8, 15, 0, 8>(U8->Q, U9->A);

    U10->CE_->feed = U8->Q[7];
    U10->OE_->feed = DataGet_;
    U10->WE_->feed = bus->VCC;
    drive_pins<8>(U10->D, U6->B);
    connect_pins<8>(U7->Q, U10->A);
    connect_pins<7, 8, 15, 0, 8>(U8->Q, U10->A);
}

Card make_Mem_Register(System &system)
{
    auto  board = system.make_board();
    auto *mem_circuit = system.circuit.add_component<Mem_Register>(system);

    board->add_device<LS138, DIP<16, Orientation::North>>(mem_circuit->U1, 8, 3, "74LS138", "U1");
    board->add_device<LS138, DIP<16, Orientation::North>>(mem_circuit->U2, 8, 21, "74LS138", "U2");
    board->add_device<LS32, DIP<14, Orientation::North>>(mem_circuit->U3, 8, 39, "74LS32", "U3");
    board->add_device<LS04, DIP<14, Orientation::North>>(mem_circuit->U4, 8, 55, "74LS04", "U4");

    board->add_device<LS245, DIP<20, Orientation::North>>(mem_circuit->U6, 18, 3, "74LS245", "U6");
    board->add_device<LS377, DIP<20, Orientation::North>>(mem_circuit->U7, 18, 25, "74LS377", "U7");
    board->add_device<LS377, DIP<20, Orientation::North>>(mem_circuit->U8, 18, 47, "74LS377", "U8");

    board->add_device<SRAM_LY62256, DIP<28, Orientation::North>>(mem_circuit->U9, 28, 3, "LY62256", "U9");
    board->add_device<EEPROM_28C256, DIP<28, Orientation::North>>(mem_circuit->U10, 28, 34, "28C256", "U10");

    auto edge = system.make_board();
    auto signals = edge->add_package<LEDArray<5, Orientation::North>>(10, 1);
    connect(std::array<Pin *, 5> { mem_circuit->DataPut_, mem_circuit->DataGet_, mem_circuit->AddrPut_, mem_circuit->Data_, mem_circuit->DataClk_ }, signals);
    edge->add_text(1, 1, "DataPut_");
    edge->add_text(1, 3, "DataGet_");
    edge->add_text(1, 5, "AddrPut_");
    edge->add_text(1, 7, "Data_");
    edge->add_text(1, 9, "DataClk_");
    signals->on_click[0] = [&system](Pin *) -> void {
        system.bus->data_transfer(0xFF, 0x07);
    };
    signals->on_click[1] = [&system](Pin *) -> void {
        system.bus->data_transfer(0x07, 0xFF);
    };
    signals->on_click[2] = [&system](Pin *) -> void {
        system.bus->addr_transfer(0xFF, 0x0F);
    };

    auto tx_dbus = edge->add_package<LEDArray<8, Orientation::North>>(10, 14);
    connect(mem_circuit->U6->B, tx_dbus);
    for (auto bit = 0; bit < 8; ++bit) {
        edge->add_text(5, 14 + 2 * bit, std::format("DQ{}", bit));
    }
    auto tx_a_lsb_bus = edge->add_package<LEDArray<8, Orientation::North>>(10, 32);
    connect(mem_circuit->U7->Q, tx_a_lsb_bus);
    for (auto bit = 0; bit < 8; ++bit) {
        edge->add_text(5, 32 + 2 * bit, std::format("AQ{}", bit));
    }
    auto tx_a_msb_bus = edge->add_package<LEDArray<8, Orientation::North>>(10, 50);
    connect(mem_circuit->U8->Q, tx_a_msb_bus);
    for (auto bit = 0; bit < 8; ++bit) {
        edge->add_text(5, 50 + 2 * bit, std::format("AQ{}", bit + 8));
    }
    return { std::move(board), std::move(edge), mem_circuit };
}

}
