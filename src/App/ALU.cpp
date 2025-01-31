/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "ALU.h"

namespace Simul {

ALU::ALU(System &system)
    : Device("ALU")
    , bus(system.bus)
{
    U1 = add_component<LS377>();
    U2 = add_component<LS245>();
    U3 = add_component<LS382>();
    U4 = add_component<LS382>();
    U5 = add_component<LS245>();
    U6 = add_component<LS138>();
    U7 = add_component<LS138>();
    U8 = add_component<LS377>();
    U9 = add_component<LS245>();
    U10 = add_component<LS08>();
    U11 = add_component<LS02>();
    U12 = add_component<LS21>();
    U13 = add_component<LS377>();
    U14 = add_component<LS157>();
    U15 = add_component<LS157>();
    U16 = add_component<LS245>();
    U17 = add_component<LS08>();
    U18 = add_component<LS00>();
    U19 = add_component<LS32>();
    U20 = add_component<LS04>();
    U21 = add_component<LS245>();

    Shift_ = U18->Y[0];
    Shift = U20->Y[1];
    LHS_ = U6->Y[4];
    RHS_ = U6->Y[5];
    Res_ = U7->Y[4];
    Flags_ = U7->Y[5];
    CFlag = U8->Q[0];
    OFlag = U8->Q[1];
    ZFlag = U8->Q[2];
    C_out = U4->Cout;
    OVR = U4->OVR;
    C = U19->Y[0];
    O = U17->Y[0];
    Z = U12->Y[0];
    F = U13->D;

    connect_pins<8>(bus->D, U1->D);
    U1->CLK->feed = bus->CLK;
    U1->E_->feed = U6->Y[4];

    connect_pins<8>(bus->D, U2->A);
    U2->DIR->feed = bus->VCC;
    U2->OE_->feed = U19->Y[2];
    drive_pins<4, 8, 4>(U2->B, U3->B);
    drive_pins<4, 8, 4, 4>(U2->B, U4->B);

    connect_pins<4, 8, 4>(U1->Q, U3->A);
    connect_pins<3, 4, 3>(bus->OP, U3->S);
    U3->Cin->feed = U10->Y[0];

    connect_pins<4, 8, 4, 4>(U1->Q, U4->A);
    connect_pins<3, 4, 3>(bus->OP, U4->S);
    U4->Cin->feed = U3->Cout;

    connect_pins<8>(U13->Q, U5->A);
    U5->DIR->feed = bus->VCC;
    U5->OE_->feed = Res_;
    drive_pins<8>(U5->B, bus->D);

    U6->A->feed = bus->PUT[0];
    U6->B->feed = bus->PUT[1];
    U6->C->feed = bus->PUT[2];
    U6->G1->feed = bus->VCC;
    U6->G2A->feed = bus->PUT[3];
    U6->G2B->feed = bus->XDATA_;

    U7->A->feed = bus->GET[0];
    U7->B->feed = bus->GET[1];
    U7->C->feed = bus->GET[2];
    U7->G1->feed = bus->VCC;
    U7->G2A->feed = bus->GET[3];
    U7->G2B->feed = bus->XDATA_;

    U8->D[0]->feed = C;
    U8->D[1]->feed = O;
    U8->D[2]->feed = U12->Y[0];
    U8->CLK->feed = bus->CLK;
    U8->E_->feed = RHS_;

    connect_pins<8>(U8->Q, U9->A);
    U9->DIR->feed = bus->VCC;
    U9->OE_->feed = Flags_;
    drive_pins<8>(U9->B, bus->D);

    U10->A[0]->feed = bus->OP[3];
    U10->B[0]->feed = CFlag;
    U10->A[1]->feed = bus->OP[0];
    U10->B[1]->feed = CFlag;
    U10->A[2]->feed = bus->OP[2];
    U10->B[2]->feed = bus->OP[3];
    U10->A[3]->feed = U10->Y[2];
    U10->B[3]->feed = U19->Y[1];

    U11->A[0]->feed = F[0];
    U11->B[0]->feed = F[1];
    U11->A[1]->feed = F[2];
    U11->B[1]->feed = F[3];
    U11->A[2]->feed = F[4];
    U11->B[2]->feed = F[5];
    U11->A[3]->feed = F[6];
    U11->B[3]->feed = F[7];

    U12->A[0]->feed = U11->Y[0];
    U12->B[0]->feed = U11->Y[1];
    U12->C[0]->feed = U11->Y[2];
    U12->D[0]->feed = U11->Y[3];

    U13->CLK->feed = bus->CLK;
    U13->E_->feed = RHS_;

    U14->I0[0]->feed = bus->D[3];
    U14->I0[1]->feed = bus->D[4];
    U14->I0[2]->feed = bus->D[5];
    U14->I0[3]->feed = bus->D[6];
    U14->I1[0]->feed = bus->D[5];
    U14->I1[1]->feed = bus->D[6];
    U14->I1[2]->feed = bus->D[7];
    U14->I1[3]->feed = U10->Y[1];
    U14->S->feed = bus->OP[1];
    U14->E_->feed = bus->GND;

    U15->I0[0]->feed = U10->Y[1];
    U15->I0[1]->feed = bus->D[0];
    U15->I0[2]->feed = bus->D[1];
    U15->I0[3]->feed = bus->D[2];
    U15->I1[0]->feed = bus->D[1];
    U15->I1[1]->feed = bus->D[2];
    U15->I1[2]->feed = bus->D[3];
    U15->I1[3]->feed = bus->D[4];
    U15->S->feed = bus->OP[1];
    U15->E_->feed = bus->GND;

    connect_pins<4, 4, 8>(U15->Z, U16->A);
    connect_pins<4, 4, 8, 0, 4>(U14->Z, U16->A);
    U16->DIR->feed = bus->VCC;
    U16->OE_->feed = Shift_;

    U17->A[0]->feed = Shift_;
    U17->B[0]->feed = OVR;
    U17->A[1]->feed = bus->OP[1];
    U17->B[1]->feed = bus->D[0];
    U17->A[2]->feed = U20->Y[0];
    U17->B[2]->feed = bus->D[7];
    U17->A[3]->feed = C_out;
    U17->B[3]->feed = Shift_;

    U18->A[0]->feed = bus->OP[2];
    U18->B[0]->feed = bus->OP[3];

    U19->A[0]->feed = U10->Y[3];
    U19->B[0]->feed = U17->Y[3];
    U19->A[1]->feed = U17->Y[1];
    U19->B[1]->feed = U17->Y[2];
    U19->A[2]->feed = RHS_;
    U19->B[2]->feed = Shift;

    U20->A[0]->feed = bus->OP[0];
    U20->A[1]->feed = Shift_;

    connect_pins<4, 4, 8>(U3->F, U21->A);
    connect_pins<4, 4, 8, 0, 4>(U4->F, U21->A);
    U21->DIR->feed = bus->VCC;
}

Card make_ALU(System &system)
{
    auto  board = system.make_board();
    auto *alu = system.circuit.add_component<ALU>(system);

    board->add_device<LS382, DIP<20, Orientation::North>>(alu->U3, 8, 3, "74LS382", "U3");
    board->add_device<LS382, DIP<20, Orientation::North>>(alu->U4, 15, 3, "74LS382", "U4");

    auto                 edge = system.make_board();
    std::array<Pin *, 5> signals = { alu->LHS_, alu->RHS_, alu->Res_, alu->Flags_, alu->Shift };
    auto                 signals_leds = leds<5>(*edge, 10, 1, signals);
    edge->add_text(1, 1, "LHS_");
    edge->add_text(1, 3, "RHS_");
    edge->add_text(1, 5, "Res_");
    edge->add_text(1, 7, "Flags_");
    edge->add_text(1, 9, "Shift");
    signals_leds->on_click[0] = [&system](Pin *) -> void {
        system.bus->data_transfer(0xFF, 0x04);
    };
    signals_leds->on_click[1] = [&system](Pin *) -> void {
        system.bus->data_transfer(0xFF, 0x05);
    };
    signals_leds->on_click[2] = [&system](Pin *) -> void {
        system.bus->data_transfer(0x04, 0xFF);
    };
    signals_leds->on_click[3] = [&system](Pin *) -> void {
        system.bus->data_transfer(0x05, 0xFF);
    };

    leds<8>(*edge, 10, 14, alu->U1->Q);
    for (auto bit = 0; bit < 8; ++bit) {
        edge->add_text(5, 14 + 2 * bit, std::format("LHS{}", bit));
    }
    leds<8>(*edge, 10, 32, alu->U2->B);
    for (auto bit = 0; bit < 8; ++bit) {
        edge->add_text(5, 32 + 2 * bit, std::format("B{}", bit));
    }
    leds<8>(*edge, 10, 50, alu->U21->A);
    for (auto bit = 0; bit < 8; ++bit) {
        edge->add_text(5, 50 + 2 * bit, std::format("F{}", bit + 8));
    }
    return { std::move(board), std::move(edge), alu };
}

}
