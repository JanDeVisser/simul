/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <raylib.h>

#include "Circuit.h"
#include "Graphics.h"
#include "LS138.h"
#include "LogicGate.h"
#include "UtilityDevice.h"

namespace Simul {

LS138::LS138()
    : Device("74LS138")
{
    auto G1inv = add_component<Inverter>();
    G1 = G1inv->A;
    auto Gnor = add_component<NorGate>(3);
    Gnor->pins[0]->feed = G1inv->Y;
    G2A = Gnor->pins[1];
    G2B = Gnor->pins[2];
    auto Ainv = add_component<Inverter>();
    A = Ainv->A;
    auto AinvInv = add_component<Inverter>();
    AinvInv->A->feed = Ainv->Y;
    auto Binv = add_component<Inverter>();
    B = Binv->A;
    auto BinvInv = add_component<Inverter>();
    BinvInv->A->feed = Binv->Y;
    auto Cinv = add_component<Inverter>();
    C = Cinv->A;
    auto CinvInv = add_component<Inverter>();
    CinvInv->A->feed = Cinv->Y;

    auto bit0 = add_component<NandGate>(4);
    bit0->A1->feed = Gnor->Y;
    bit0->A2->feed = Ainv->Y;
    bit0->pins[2]->feed = Binv->Y;
    bit0->pins[3]->feed = Cinv->Y;
    Y[0] = bit0->Y;
    auto bit1 = add_component<NandGate>(4);
    bit1->A1->feed = Gnor->Y;
    bit1->A2->feed = AinvInv->Y;
    bit1->pins[2]->feed = Binv->Y;
    bit1->pins[3]->feed = Cinv->Y;
    Y[1] = bit1->Y;
    auto bit2 = add_component<NandGate>(4);
    bit2->A1->feed = Gnor->Y;
    bit2->A2->feed = Ainv->Y;
    bit2->pins[2]->feed = BinvInv->Y;
    bit2->pins[3]->feed = Cinv->Y;
    Y[2] = bit2->Y;
    auto bit3 = add_component<NandGate>(4);
    bit3->A1->feed = Gnor->Y;
    bit3->A2->feed = AinvInv->Y;
    bit3->pins[2]->feed = BinvInv->Y;
    bit3->pins[3]->feed = Cinv->Y;
    Y[3] = bit3->Y;

    auto bit4 = add_component<NandGate>(4);
    bit4->A1->feed = Gnor->Y;
    bit4->A2->feed = Ainv->Y;
    bit4->pins[2]->feed = Binv->Y;
    bit4->pins[3]->feed = CinvInv->Y;
    Y[4] = bit4->Y;
    auto bit5 = add_component<NandGate>(4);
    bit5->A1->feed = Gnor->Y;
    bit5->A2->feed = AinvInv->Y;
    bit5->pins[2]->feed = Binv->Y;
    bit5->pins[3]->feed = CinvInv->Y;
    Y[5] = bit5->Y;
    auto bit6 = add_component<NandGate>(4);
    bit6->A1->feed = Gnor->Y;
    bit6->A2->feed = Ainv->Y;
    bit6->pins[2]->feed = BinvInv->Y;
    bit6->pins[3]->feed = CinvInv->Y;
    Y[6] = bit6->Y;
    auto bit7 = add_component<NandGate>(4);
    bit7->A1->feed = Gnor->Y;
    bit7->A2->feed = AinvInv->Y;
    bit7->pins[2]->feed = BinvInv->Y;
    bit7->pins[3]->feed = CinvInv->Y;
    Y[7] = bit7->Y;
}

void ls138_test(Board &board)
{
    board.circuit.name = "LS138 Test";
    auto *ls138 = board.circuit.add_component<LS138>();
    board.add_device<LS138, DIP<16, Orientation::North>>(ls138, Vector2 { 10, 6 });
    std::array<TieDown *, 6> in {};
    for (auto ix = 0; ix < 6; ++ix) {
        in[ix] = board.circuit.add_component<TieDown>(PinState::Low);
    }
    ls138->G1->feed = in[0]->Y;
    ls138->G2A->feed = in[1]->Y;
    ls138->G2B->feed = in[2]->Y;
    ls138->A->feed = in[3]->Y;
    ls138->B->feed = in[4]->Y;
    ls138->C->feed = in[5]->Y;

    auto S = board.add_package<DIPSwitch<6, Orientation::North>>(Vector2 { 2.0f, 3 });
    connect(in, S);
    auto L = board.add_package<LEDArray<8, Orientation::North>>(Vector2 { 17, 3 });
    connect(ls138->Y, L);
}

}
