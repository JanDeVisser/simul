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
    U1 = add_component<LS138>();
    U2 = add_component<LS138>();
    U3 = add_component<LS138>();
    U4 = add_component<LS32>();
    U5 = add_component<LS08>();
    U6 = add_component<LS32>();
    U7 = add_component<LS00>();
    U8 = add_component<LS139>();
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
    U2->G1->feed = bus->PUT[3];
    U2->G2A->feed = Circuit::the().GND;
    U2->G2B->feed = bus->GET[2];

    Get_ = U2->Y[reg_no - 8];
    U6->A[0]->feed = bus->XDATA_;
    U6->B[0]->feed = Get_;
    DGet_ = U6->Y[0];
    U6->A[1]->feed = bus->XADDR_;
    U6->B[1]->feed = Get_;
    AGet_ = U4->Y[1];

    U3->A->feed = MSB;
    U3->B->feed = DPut_;
    U3->C->feed = DGet_;
    U3->G1 = Circuit::the().VCC;
    U3->G2A = bus->XDATA_;
    U3->G2B = Circuit::the().GND;

    MSBGet_ = U3->Y[3];
    U5->A[0]->feed = U3->Y[2];
    U5->B[0]->feed = AGet_;
    LSBGet_ = U5->Y[0];
    U5->A[1]->feed = U3->Y[4];
    U5->B[1]->feed = APut_;
    LSBPut_ = U5->Y[0];
    U5->A[2]->feed = U3->Y[5];
    U5->B[2]->feed = APut_;
    MSBPut_ = U5->Y[0];
    U6->A[1]->feed = LSBPut_;
    U6->B[1]->feed = bus->CLK_;
    LSBLoad_ = U6->Y[1];
    U6->A[2]->feed = MSBPut_;
    U6->B[2]->feed = bus->CLK_;
    MSBLoad_ = U6->Y[2];

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

    U10->Load_->feed = LSBLoad_;
    U10->Up->feed = Increment;
    U10->Down->feed = Decrement;
    U10->CLR->feed = bus->RST;
    for (auto bit = 0; bit < 4; ++bit) {
        U10->D[bit]->feed = bus->D[bit];
    }

    U11->Load_->feed = LSBLoad_;
    U11->Up->feed = U10->CO_;
    U11->Down->feed = U10->BO_;
    U11->CLR->feed = bus->RST;
    for (auto bit = 0; bit < 4; ++bit) {
        U11->D[bit]->feed = bus->D[bit + 4];
    }

    U12->Load_->feed = MSBLoad_;
    U12->Up->feed = U11->CO_;
    U12->Down->feed = U11->BO_;
    U12->CLR->feed = bus->RST;
    for (auto bit = 0; bit < 4; ++bit) {
        U10->D[bit]->feed = U17->Z[bit];
    }

    U13->Load_->feed = MSBLoad_;
    U13->Up->feed = U11->CO_;
    U13->Down->feed = U11->BO_;
    U13->CLR->feed = bus->RST;
    for (auto bit = 0; bit < 4; ++bit) {
        U10->D[bit]->feed = U18->Z[bit];
    }

    U14->DIR->feed = Circuit::the().GND;
    U14->OE_->feed = LSBGet_;
    for (auto bit = 0; bit < 4; ++bit) {
        U14->A[bit]->drive = bus->D[bit];
        U14->B[bit]->feed = U10->Q[bit];
    }
    for (auto bit = 4; bit < 8; ++bit) {
        U14->A[bit]->drive = bus->D[bit];
        U14->B[bit]->feed = U11->Q[bit - 4];
    }

    U15->DIR->feed = Circuit::the().GND;
    U15->OE_->feed = MSBGet_;
    for (auto bit = 0; bit < 4; ++bit) {
        U15->A[bit]->drive = bus->D[bit];
        U15->B[bit]->feed = U12->Q[bit];
    }
    for (auto bit = 4; bit < 8; ++bit) {
        U15->A[bit]->drive = bus->D[bit];
        U15->B[bit]->feed = U13->Q[bit - 4];
    }

    U16->DIR->feed = Circuit::the().GND;
    U16->OE_->feed = AGet_;
    for (auto bit = 0; bit < 4; ++bit) {
        U16->A[bit]->drive = bus->ADDR[bit];
        U16->B[bit]->feed = U12->Q[bit];
    }
    for (auto bit = 4; bit < 8; ++bit) {
        U16->A[bit]->drive = bus->ADDR[bit];
        U16->B[bit]->feed = U13->Q[bit - 4];
    }

    U17->S = DPut_;
    U17->E_ = MSBPut_;
    for (auto bit = 0; bit < 4; ++bit) {
        U17->I0[bit]->feed = bus->D[bit];
        U17->I1[bit]->feed = bus->ADDR[bit];
    }
    U18->S = DPut_;
    U18->E_ = MSBPut_;
    for (auto bit = 0; bit < 4; ++bit) {
        U17->I0[bit]->feed = bus->D[bit + 4];
        U17->I1[bit]->feed = bus->ADDR[bit + 4];
    }
}

}
