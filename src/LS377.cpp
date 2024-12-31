/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <raylib.h>

#include "Circuit.h"
#include "Graphics.h"
#include "Latch.h"
#include "LS377.h"
#include "LogicGate.h"
#include "Oscillator.h"
#include "UtilityDevice.h"

namespace Simul {

// NOTE Does not work right now

LS377::LS377()
    : Device("74LS377 - Octal D-Type Flip-Flop with Common Enable and Clock", "74LS377")
{
    auto *Einv = add_component<Inverter>();
    E = Einv->A;
    auto *CLKinv = add_component<Inverter>();
    CLK = CLKinv->A;
    for (auto bit = 0; bit < 8; ++bit) {
        auto *latch = add_component<DFlipFlop>();
        auto *Dand = add_component<AndGate>();
        D[bit] = Dand->A1;
        Dand->A2->feed = Einv->Y;
        auto *curAnd = add_component<AndGate>();
        curAnd->A1->feed = latch->Q;
        curAnd->A2->feed = Einv->Y;
        auto *nor = add_component<NorGate>();
        nor->A1->feed = Dand->Y;
        nor->A2->feed = curAnd->Y;
        latch->D->feed = nor->Y;
        latch->CLK->feed = CLKinv->Y;
        Q[bit] = latch->Q;
    }
}

void LS377_test(Board &board)
{
    board.circuit.name = "LS377 Test";
    auto *clock = board.circuit.add_component<Oscillator>(1);
    board.add_device<Oscillator,OscillatorIcon>(clock, Vector2 { 10, 19 });
    auto *ls377 = board.circuit.add_component<LS377>();
    board.add_device<LS377, DIP<20, Orientation::North>>(ls377, Vector2 { 10, 6 });
    ls377->CLK->feed = clock->Y;

    auto *CE_S = board.add_package<DIPSwitch<1, Orientation::North>>(Vector2 { 2, 2 });
    connect(ls377->E, CE_S);
    auto *S = board.add_package<DIPSwitch<8, Orientation::North>>(Vector2 { 2, 7 });
    connect(ls377->D, S);
    auto *L = board.add_package<LEDArray<8, Orientation::North>>(Vector2 { 17, 3 });
    connect(ls377->Q, L);
}

}
