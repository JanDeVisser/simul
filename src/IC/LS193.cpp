/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "LS193.h"
#include "Circuit/Latch.h"
#include "Circuit/LogicGate.h"

namespace Simul {

LS193::Latch::Latch(LS193 *chip, int bit)
    : Device("74LS193 Channel Latch")
{
    SetNand = add_component<NandGate>(3);
    D = SetNand->A1;
    SetNand->A2->feed = chip->CLR_;
    SetNand->pin(3)->feed = chip->Load;
    LoadNand = add_component<NandGate>();
    LoadNand->A1->feed = SetNand->Y;
    LoadNand->A2->feed = chip->Load;
    ResetOr = add_component<OrGate>();
    invert(chip->CLR_, ResetOr->A1);
    invert(LoadNand->Y, ResetOr->A2);
    ClockOr = add_component<OrGate>();
    latch = add_component<TFlipFlop>();
    ClockOr->A1->feed = chip->Up_;
    ClockOr->A2->feed = chip->Down_;

    if (bit > 0) {
        clockBorrowAnd = add_component<AndGate>(bit + 1);
        clockCarryAnd = add_component<AndGate>(bit + 1);
        clockBorrowAnd->pin(bit + 1)->feed = chip->Down_;
        clockCarryAnd->pin(bit + 1)->feed = chip->Up_;
        for (auto ix = 0; ix < bit; ++ix) {
            clockBorrowAnd->pin(ix + 1)->feed = chip->latches[ix]->Q_;
            clockCarryAnd->pin(ix + 1)->feed = chip->latches[ix]->Q;
        }
        ClockOr->A1->feed = clockBorrowAnd->Y;
        ClockOr->A2->feed = clockCarryAnd->Y;
    }

    invert(ClockOr->Y, latch->CLK);
    latch->T->feed = Circuit::the().VCC;
    invert(ResetOr->Y, latch->CLR_);
    latch->SET_->feed = SetNand->Y;
    Q = latch->Q;
    Q_ = latch->Q_;
}

LS193::LS193()
    : Device("74LS193 - Synchronous 4 bit up/down counters (dual clock with clear)")
{
    auto *UpInv = add_component<Inverter>();
    Up = UpInv->A;
    Up_ = UpInv->Y;
    auto *DownInv = add_component<Inverter>();
    Down = DownInv->A;
    Down_ = DownInv->Y;
    auto *ClrInv = add_component<Inverter>();
    CLR = ClrInv->A;
    CLR_ = ClrInv->Y;
    auto *LoadInv = add_component<Inverter>();
    Load_ = LoadInv->A;
    Load = LoadInv->Y;
    BONand = add_component<NandGate>(5);
    CONand = add_component<NandGate>(5);
    BONand->pin(5)->feed = Down_;
    CONand->pin(5)->feed = Up_;
    BO_ = BONand->Y;
    CO_ = CONand->Y;

    for (auto bit = 0; bit < 4; ++bit) {
        latches[bit] = add_component<Latch>(this, bit);
        BONand->pin(bit + 1)->feed = latches[bit]->Q_;
        CONand->pin(bit + 1)->feed = latches[bit]->Q;
        D[bit] = latches[bit]->D;
        Q[bit] = latches[bit]->Q;
    }
}

void LS193::test_setup(Circuit &circuit)
{
    Up->state = PinState::High;
    Down->state = PinState::High;
    CLR->state = PinState::Low;
    Load_->state = PinState::Low;
    set_pins(D, 0x00);
}

void LS193::test_run(Circuit &circuit)
{
    assert(Q[0]->off());
    assert(Q[1]->off());
    assert(Q[2]->off());
    assert(Q[3]->off());
    Load_->state = PinState::High;
    circuit.yield();
    set_pins(D, 0x01);
    Load_->state = PinState::Low;
    circuit.yield();
    assert(Q[0]->on());
    assert(Q[1]->off());
    assert(Q[2]->off());
    assert(Q[3]->off());
    set_pins(D, 0x02);
    circuit.yield();
    assert(Q[0]->off());
    assert(Q[1]->on());
    assert(Q[2]->off());
    assert(Q[3]->off());
    set_pins(D, 0x04);
    circuit.yield();
    assert(Q[0]->off());
    assert(Q[1]->off());
    assert(Q[2]->on());
    assert(Q[3]->off());
    Load_->state = PinState::High;
    Up->state = PinState::Low;
    circuit.yield();
    Up->state = PinState::High;
    circuit.yield();
    assert(Q[0]->on());
    assert(Q[1]->off());
    assert(Q[2]->on());
    assert(Q[3]->off());
    Up->state = PinState::Low;
    circuit.yield();
}

LS193_Bit0::LS193_Bit0()
    : Device("74LS193 Bit 0")
{
    auto *UpInv = add_component<Inverter>();
    Up = UpInv->A;
    auto *Up_ = UpInv->Y;
    auto *DownInv = add_component<Inverter>();
    Down = DownInv->A;
    auto *Down_ = DownInv->Y;
    auto *ClrInv = add_component<Inverter>();
    CLR = ClrInv->A;
    auto *CLR_ = ClrInv->Y;
    auto *LoadInv = add_component<Inverter>();
    Load_ = LoadInv->A;
    auto *Load = LoadInv->Y;
    auto *BONand = add_component<NandGate>();
    auto *CONand = add_component<NandGate>();
    BONand->A1->feed = Down_;
    CONand->A1->feed = Up_;
    BO_ = BONand->Y;
    CO_ = CONand->Y;

    SetNand = add_component<NandGate>(3);
    D = SetNand->A1;
    SetNand->A2->feed = CLR_;
    SetNand->pin(3)->feed = Load;
    LoadNand = add_component<NandGate>();
    LoadNand->A1->feed = SetNand->Y;
    LoadNand->A2->feed = Load;
    ResetOr = add_component<OrGate>();
    invert(CLR_, ResetOr->A1);
    invert(LoadNand->Y, ResetOr->A2);
    ClockOr = add_component<OrGate>();
    latch = add_component<TFlipFlop>();
    ClockOr->A1->feed = Up_;
    ClockOr->A2->feed = Down_;
    invert(ClockOr->Y, latch->CLK);
    latch->T->feed = add_pin(1, "VCC", PinState::High);
    invert(ResetOr->Y, latch->CLR_);
    latch->SET_->feed = SetNand->Y;
    BONand->A2->feed = latch->Q_;
    CONand->A2->feed = latch->Q;
    Q = latch->Q;
}

}
