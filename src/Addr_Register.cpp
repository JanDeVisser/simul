/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "Addr_Register.h"
#include "System.h"

namespace Simul {

Addr_Register::Addr_Register(System &system, int reg_no)
    : Device(std::format("Address Register {}", reg_no))
    , bus(system.bus)
    , reg_no(reg_no)
{
    assert(8 <= reg_no && reg_no < 13);
    switch (reg_no) {
    case 8:
        name = "PC";
        break;
    case 9:
        name = "SP";
        break;
    case 10:
        name = "Si";
        break;
    case 11:
        name = "Di";
        break;
    case 12:
        name = "TX";
        break;
    default:
        assert("Unreachable" != nullptr);
        break;
    }
    U1 = add_component<LS138>();
    U2 = add_component<LS138>();
    U3 = add_component<LS138>();
    U4 = add_component<LS32>();
    U5 = add_component<LS08>();
    U6 = add_component<LS32>();
    U7 = add_component<LS00>();
    U8 = add_component<LS139>();
    U10 = add_component<LS193>();
    U11 = add_component<LS193>();
    U12 = add_component<LS193>();
    U13 = add_component<LS193>();
    U14 = add_component<LS245>();
    U15 = add_component<LS245>();
    U16 = add_component<LS245>();
    U17 = add_component<LS157>();
    U18 = add_component<LS157>();

    MSB = bus->OP[3];

    U1->A->feed = bus->PUT[0];
    U1->B->feed = bus->PUT[1];
    U1->C->feed = Circuit::the().GND;
    U1->G1->feed = bus->PUT[3];
    U1->G2A->feed = Circuit::the().GND;
    U1->G2B->feed = bus->PUT[2];

    Put_ = U1->Y[reg_no - 8];
    U4->A[0]->feed = bus->XDATA_;
    U4->B[0]->feed = Put_;
    DPut_ = U4->Y[0];
    U4->A[1]->feed = bus->XADDR_;
    U4->B[1]->feed = Put_;
    APut_ = U4->Y[1];

    U2->A->feed = bus->GET[0];
    U2->B->feed = bus->GET[1];
    U2->C->feed = Circuit::the().GND;
    U2->G1->feed = bus->GET[3];
    U2->G2A->feed = Circuit::the().GND;
    U2->G2B->feed = bus->GET[2];

    Get_ = U2->Y[reg_no - 8];
    U6->A[0]->feed = bus->XDATA_;
    U6->B[0]->feed = Get_;
    DGet_ = U6->Y[0];
    U6->A[1]->feed = bus->XADDR_;
    U6->B[1]->feed = Get_;
    AGet_ = U6->Y[1];

    U3->A->feed = MSB;
    U3->B->feed = DPut_;
    U3->C->feed = DGet_;
    U3->G1->feed = Circuit::the().VCC;
    U3->G2A->feed = bus->XDATA_;
    U3->G2B->feed = Circuit::the().GND;

    MSBGet_ = U3->Y[3];
    U5->A[0]->feed = U3->Y[2];
    U5->B[0]->feed = AGet_;
    LSBGet_ = U5->Y[0];
    U5->A[1]->feed = U3->Y[4];
    U5->B[1]->feed = APut_;
    LSBPut_ = U5->Y[1];
    U5->A[2]->feed = U3->Y[5];
    U5->B[2]->feed = APut_;
    MSBPut_ = U5->Y[2];
    U6->A[2]->feed = LSBPut_;
    U6->B[2]->feed = bus->CLK_;
    LSBLoad_ = U6->Y[2];
    U6->A[3]->feed = MSBPut_;
    U6->B[3]->feed = bus->CLK_;
    MSBLoad_ = U6->Y[3];

    U8->A[0]->feed = bus->OP[0];
    U8->B[0]->feed = bus->OP[1];
    U8->G[0]->feed = bus->OP[2];

    U4->A[3]->feed = bus->CLK_;
    U4->B[3]->feed = Get_;
    U7->A[0]->feed = U4->Y[3];
    U7->B[0]->feed = U8->Y1[0];
    Increment = U7->Y[0];
    U7->A[1]->feed = U4->Y[3];
    U7->B[1]->feed = U8->Y2[0];
    Decrement = U7->Y[0];

    U10->Load_->feed = U11->Load_->feed = LSBLoad_;
    U12->Load_->feed = U13->Load_->feed = MSBLoad_;
    U10->CLR->feed = U11->CLR->feed = U12->CLR->feed = U13->CLR->feed = bus->RST;

    U10->Up->feed = Increment;
    U10->Down->feed = Decrement;
    U11->Up->feed = U10->CO_;
    U11->Down->feed = U10->BO_;
    U12->Up->feed = U11->CO_;
    U12->Down->feed = U11->BO_;
    U13->Up->feed = U12->CO_;
    U13->Down->feed = U12->BO_;

    connect_pins<4, 8, 4>(bus->D, U10->D);
    connect_pins<4, 4, 8>(U10->Q, U14->B);
    connect_pins<4, 8, 4, 4>(bus->D, U11->D);
    connect_pins<4, 4, 8, 0, 4>(U11->Q, U14->B);
    connect_pins<4>(U17->Z, U12->D);
    connect_pins<4, 4, 8>(U12->Q, U15->B);
    connect_pins<4>(U18->Z, U13->D);
    connect_pins<4, 4, 8, 0, 4>(U13->Q, U15->B);
    connect_pins<8>(U15->B, U16->B);

    U14->DIR->feed = U15->DIR->feed = U16->DIR->feed = Circuit::the().GND;
    U14->OE_->feed = LSBGet_;
    U15->OE_->feed = MSBGet_;
    U16->OE_->feed = AGet_;

    drive_pins<8>(U14->A, bus->D);
    drive_pins<8>(U15->A, bus->D);
    drive_pins<8>(U16->A, bus->ADDR);

    U17->S->feed = U18->S->feed = DPut_;
    U17->E_->feed = U18->E_->feed = MSBPut_;
    connect_pins<4, 8, 4>(bus->D, U17->I0);
    connect_pins<4, 8, 4>(bus->ADDR, U17->I1);
    connect_pins<4, 8, 4, 4>(bus->D, U18->I0);
    connect_pins<4, 8, 4, 4>(bus->ADDR, U18->I1);
}

Card make_Addr_Register(System &system, int reg_no)
{
    auto  board = system.make_board();
    auto *reg_circuit = system.circuit.add_component<Addr_Register>(system, reg_no);
    bus_label(*board, 3, "MSB");

    board->add_device<LS138, DIP<16, Orientation::North>>(reg_circuit->U1, 8, 3, "74LS138", "U1");
    board->add_device<LS138, DIP<16, Orientation::North>>(reg_circuit->U2, 8, 21, "74LS138", "U2");
    board->add_device<LS138, DIP<16, Orientation::North>>(reg_circuit->U3, 8, 39, "74LS138", "U3");
    board->add_device<LS139, DIP<16, Orientation::North>>(reg_circuit->U8, 8, 57, "74LS139", "U8");

    board->add_device<LS32, DIP<14, Orientation::North>>(reg_circuit->U4, 18, 3, "74LS32", "U4");
    board->add_device<LS08, DIP<14, Orientation::North>>(reg_circuit->U5, 18, 19, "74LS08", "U5");
    board->add_device<LS32, DIP<14, Orientation::North>>(reg_circuit->U6, 18, 35, "74LS32", "U6");
    board->add_device<LS00, DIP<14, Orientation::North>>(reg_circuit->U7, 18, 51, "74LS00", "U7");

    board->add_device<LS193, DIP<16, Orientation::North>>(reg_circuit->U10, 28, 3, "74LS193", "U10");
    board->add_device<LS193, DIP<16, Orientation::North>>(reg_circuit->U10, 28, 21, "74LS193", "U11");
    board->add_device<LS193, DIP<16, Orientation::North>>(reg_circuit->U10, 28, 39, "74LS193", "U12");
    board->add_device<LS193, DIP<16, Orientation::North>>(reg_circuit->U10, 28, 57, "74LS193", "U13");

    board->add_device<LS157, DIP<16, Orientation::North>>(reg_circuit->U17, 48, 3, "74LS157", "U17");
    board->add_device<LS157, DIP<16, Orientation::North>>(reg_circuit->U18, 48, 21, "74LS157", "U18");

    auto edge = system.make_board();
    auto signals = edge->add_package<LEDArray<6, Orientation::North>>(10, 1);
    connect(std::array<Pin *, 6> { reg_circuit->LSBPut_, reg_circuit->MSBPut_, reg_circuit->APut_, reg_circuit->LSBGet_, reg_circuit->MSBGet_, reg_circuit->AGet_ }, signals);
    edge->add_text(1, 1, "LSBPut_");
    edge->add_text(1, 3, "MSBPut_");
    edge->add_text(1, 5, "APut_");
    edge->add_text(1, 7, "LSBGet_");
    edge->add_text(1, 9, "MSBGet_");
    edge->add_text(1, 11, "AGet_");
    signals->on_click[0] = [&system, reg_no](Pin *) -> void {
        set_pins(system.bus->PUT, static_cast<uint8_t>(reg_no));
        system.bus->XDATA_->state = PinState::Low;
        set_pins(system.bus->OP, 0x0);
    };
    signals->on_click[1] = [&system, reg_no](Pin *) -> void {
        set_pins(system.bus->PUT, static_cast<uint8_t>(reg_no));
        system.bus->XDATA_->state = PinState::Low;
        set_pins(system.bus->OP, 0x8);
    };
    signals->on_click[2] = [&system, reg_no](Pin *) -> void {
        set_pins(system.bus->PUT, static_cast<uint8_t>(reg_no));
        system.bus->XADDR_->state = PinState::Low;
    };
    signals->on_click[3] = [&system, reg_no](Pin *) -> void {
        set_pins(system.bus->GET, static_cast<uint8_t>(reg_no));
        system.bus->XDATA_->state = PinState::Low;
        set_pins(system.bus->OP, 0x0);
    };
    signals->on_click[4] = [&system, reg_no](Pin *) -> void {
        set_pins(system.bus->GET, static_cast<uint8_t>(reg_no));
        system.bus->XDATA_->state = PinState::Low;
        set_pins(system.bus->OP, 0x8);
    };
    signals->on_click[5] = [&system, reg_no](Pin *) -> void {
        set_pins(system.bus->GET, static_cast<uint8_t>(reg_no));
        system.bus->XADDR_->state = PinState::Low;
    };

    auto tx_dbus = edge->add_package<LEDArray<8, Orientation::North>>(10, 14);
    connect(reg_circuit->U14->B, tx_dbus);
    for (auto bit = 0; bit < 8; ++bit) {
        edge->add_text(5, 14 + 2 * bit, std::format("DQ{}", bit));
    }
    auto tx_abus = edge->add_package<LEDArray<8, Orientation::North>>(10, 32);
    connect(reg_circuit->U15->B, tx_abus);
    for (auto bit = 0; bit < 8; ++bit) {
        edge->add_text(5, 32 + 2 * bit, std::format("AQ{}", bit));
    }
    board->add_text(2, 80, reg_circuit->name);
    edge->add_text(2, 80, reg_circuit->name);
    return { std::move(board), std::move(edge), reg_circuit };
}

}
