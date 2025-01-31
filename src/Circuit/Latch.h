/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Device.h"
#include "Graphics.h"
#include "LogicGate.h"

namespace Simul {

struct SRLatch : public Device {
    Pin *S_;
    Pin *R_;
    Pin *Q;
    Pin *Q_;

    NandGate *S_Gate;
    NandGate *R_Gate;

    SRLatch(int inputs = 1);
    void test_setup(Circuit &) override;
    void test_run(Circuit &) override;
};

template<size_t Inputs = 1>
struct GatedSRLatch : public Device {
    std::array<Pin *, Inputs> S_ {};
    std::array<Pin *, Inputs> R_ {};
    Pin                      *E;
    Pin                      *Q;
    Pin                      *Q_;
    Pin                      *CLR_;
    Pin                      *SET_;

    GatedSRLatch()
        : Device("Gated S/R Latch")
    {
        Snand = add_component<NandGate>();
        Rnand = add_component<NandGate>();
        setAnd = add_component<AndGate>();
        clrAnd = add_component<AndGate>();
        SgateNand = add_component<NandGate>(static_cast<int>(Inputs + 1));
        RgateNand = add_component<NandGate>(static_cast<int>(Inputs + 1));

        for (auto input = 1; input <= Inputs; ++input) {
            S_[input - 1] = SgateNand->pin(input);
            R_[input - 1] = RgateNand->pin(input);
        }
        E = RgateNand->pin(Inputs + 1);
        SgateNand->pin(Inputs + 1)->feed = E;

        setAnd->A1->feed = SgateNand->Y;
        SET_ = setAnd->A2;
        setAnd->A2->feed = Circuit::the().VCC;

        clrAnd->A1->feed = RgateNand->Y;
        CLR_ = clrAnd->A2;
        clrAnd->A2->feed = Circuit::the().VCC;

        Snand->A1->feed = setAnd->Y;
        Snand->A2->feed = Rnand->Y;
        Rnand->A1->feed = clrAnd->Y;
        Rnand->A2->feed = Snand->Y;
        Q = Snand->Y;
        Q_ = Rnand->Y;
    }

    void test_setup(Circuit &) override
    {
        S_[0]->state = PinState::Low;
        R_[0]->state = PinState::High;
        E->state = PinState::High;
    }

    void test_run(Circuit &circuit) override
    {
        assert(Q->state != Q_->state);
        auto q = Q->state;
        E->state = PinState::Low;
        S_[0]->state = PinState::High;
        R_[0]->state = PinState::Low;
        circuit.yield();
        assert(Q->state == q);
        E->state = PinState::High;
        circuit.yield();
        assert(Q->state != q);
    }

private:
    NandGate *Rnand;
    NandGate *Snand;
    AndGate  *clrAnd;
    AndGate  *setAnd;
    NandGate *RgateNand;
    NandGate *SgateNand;
};

struct DFlipFlop : public Device {
    Pin *CLK;
    Pin *D;
    Pin *SET_;
    Pin *CLR_;
    Pin *Q;
    Pin *Q_;

    DFlipFlop();
    void test_run(Circuit &) override;

private:
    SRLatch *output;
    SRLatch *d_input;
    SRLatch *a_input;
};

struct DFlipFlopIcon : Package<3> {
    explicit DFlipFlopIcon(Vector2 pos);
    void render() override;
    void handle_input() override;
};

/**
 * From SN74-76A datasheet:
 *
 * SET_ CLR_ CLK J K  | Q  Q_
 * ---------------------------
 *  L    H    X  X X  ︎   H  L
 *  H    L    X  X X  ︎   L  H
 *  L    L    X  X X  ︎   -  -  (unstable)
 *  H    H    ⬇  L L     Q  Q_ ︎
 *  H    H    ⬇  H L     H  L ︎
 *  H    H    ⬇  L H     L  H ︎
 *  H    H    ⬇  H H     Q_ Q (toggle) ︎
 *  H    H    H  X X     Q  Q_
 */

struct JKFlipFlop : public Device {
    Pin *CLK;
    Pin *J;
    Pin *K;
    Pin *SET_;
    Pin *CLR_;
    Pin *Q;
    Pin *Q_;

    JKFlipFlop();
    void test_setup(Circuit &) override;
    void test_run(Circuit &) override;

    NandGate *J_gate;
    AndGate  *set;
    NandGate *K_gate;
    AndGate  *clr;
    SRLatch  *secondary;
};

struct JKFlipFlopIcon : Package<5> {
    explicit JKFlipFlopIcon(Vector2 pos);
    void render() override;
    void handle_input() override;
};

struct TFlipFlop : public Device {
    Pin *CLK;
    Pin *T;
    Pin *SET_;
    Pin *CLR_;
    Pin *Q;
    Pin *Q_;

    TFlipFlop();

    DFlipFlop *flip_flop;
    XorGate   *toggle;
};

struct TFlipFlopIcon : Package<6> {
    explicit TFlipFlopIcon(Vector2 pos);
    void render() override;
    void handle_input() override;
};

template<>
inline void connect(DFlipFlop *device, DFlipFlopIcon *package)
{
    package->pins[0] = device->D;
    package->pins[1] = device->CLK;
    package->pins[2] = device->Q;
}

template<>
inline void connect(JKFlipFlop *device, JKFlipFlopIcon *package)
{
    package->pins[0] = device->J;
    package->pins[1] = device->K;
    package->pins[2] = device->CLK;
    package->pins[3] = device->Q;
    package->pins[4] = device->Q_;
}

template<>
inline void connect(TFlipFlop *device, TFlipFlopIcon *package)
{
    package->pins[0] = device->T;
    package->pins[1] = device->CLK;
    package->pins[2] = device->SET_;
    package->pins[3] = device->CLR_;
    package->pins[4] = device->Q_;
    package->pins[5] = device->Q_;
}

void SRLatch_test(Board &);
void DFlipFlop_test(Board &);

}
