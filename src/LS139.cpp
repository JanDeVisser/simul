/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <raylib.h>

#include "Circuit.h"
#include "Graphics.h"
#include "LS139.h"
#include "LogicGate.h"
#include "UtilityDevice.h"

namespace Simul {

LS139::LS139()
    : Device("74LS139")
{
    for (auto ix = 0; ix < 2; ++ix) {
        auto decoder = add_component<Device>("2-to-4 decoder/multiplexer");
        auto Ginv = decoder->add_component<Inverter>();
        auto Ainv = decoder->add_component<Inverter>();
        auto Binv = decoder->add_component<Inverter>();
        auto AinvInv = decoder->add_component<Inverter>();
        auto BinvInv = decoder->add_component<Inverter>();
        G[ix] = Ginv->A;
        A[ix] = Ainv->A;
        B[ix] = Binv->A;
        AinvInv->A->feed = Ainv->Y;
        BinvInv->A->feed = Binv->Y;
        auto bit0 = decoder->add_component<NandGate>(3);
        bit0->A1->feed = Ginv->Y;
        bit0->A2->feed = Ainv->Y;
        bit0->pins[2]->feed = Binv->Y;
        Y0[ix] = bit0->Y;
        auto bit1 = decoder->add_component<NandGate>(3);
        bit1->A1->feed = Ginv->Y;
        bit1->A2->feed = AinvInv->Y;
        bit1->pins[2]->feed = Binv->Y;
        Y1[ix] = bit1->Y;
        auto bit2 = decoder->add_component<NandGate>(3);
        bit2->A1->feed = Ginv->Y;
        bit2->A2->feed = Ainv->Y;
        bit2->pins[2]->feed = BinvInv->Y;
        Y2[ix] = bit2->Y;
        auto bit3 = decoder->add_component<NandGate>(3);
        bit3->A1->feed = Ginv->Y;
        bit3->A2->feed = AinvInv->Y;
        bit3->pins[2]->feed = BinvInv->Y;
        Y3[ix] = bit3->Y;
    }
}

void LS139_test(Board &board)
{
    board.circuit.name = "LS139 Test";
    auto *ls139 = board.circuit.add_component<LS139>();
    board.add_device<LS139, DIP<16, Orientation::North>>(ls139, Vector2 { 10, 6 });
    for (auto decoder = 0; decoder < 2; ++decoder) {
        std::array<TieDown *, 2> in {};
        for (auto ix = 0; ix < 2; ++ix) {
            in[ix] = board.circuit.add_component<TieDown>(PinState::Low);
        }
        ls139->G[decoder]->feed = board.circuit.GND;
        ls139->A[decoder]->feed = in[0]->Y;
        ls139->B[decoder]->feed = in[1]->Y;

        auto S = board.add_package<DIPSwitch<2, Orientation::North>>(Vector2 { 3 + decoder*14.0f, 3 });
        connect(in, S);
        auto L = board.add_package<LEDArray<4, Orientation::North>>(Vector2 { 4 + decoder*14.0f, 10 });
        connect(std::array<Pin *, 4> { ls139->Y0[decoder], ls139->Y1[decoder], ls139->Y2[decoder], ls139->Y3[decoder] }, L);
    }
}

}
