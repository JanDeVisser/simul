/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <raylib.h>

#include "Circuit.h"
#include "Graphics.h"
#include "LS245.h"
#include "LogicGate.h"
#include "UtilityDevice.h"

namespace Simul {

LS245::Channel::Channel()
    : Device("Single LS245 channel")
{
    A = add_component<TristatePin>();
    B = add_component<TristatePin>();
    Abuf = add_component<TriStateBuffer>();
    Bbuf = add_component<TriStateBuffer>();
    auto *DirInv = add_component<Inverter>();
    DIR = DirInv->A;
    AE = Abuf->E;
    BE = Bbuf->E;
    Abuf->A->feed = A->I;
    Bbuf->A->feed = B->I;

    A->I->feed = Bbuf->Y;
    A->DIR->feed = DIR;
    B->I->feed = Abuf->Y;
    B->DIR->feed = DirInv->Y;
}

LS245::LS245()
    : Device("74LS245 - Octal Bus Transceivers With 3-State Outputs", "74LS245")
{
    OEinv = add_component<Inverter>();
    ASide = add_component<AndGate>();
    BSide = add_component<NorGate>();
    OE_ = OEinv->A;
    DIR = ASide->A1;
    ASide->A2->feed = OEinv->Y;
    BSide->A1->feed = ASide->A1;
    BSide->A2->feed = OE_;
    for (auto bit = 0; bit < 8; ++bit) {
        channels[bit] = add_component<Channel>();
        channels[bit]->AE->feed = ASide->Y;
        channels[bit]->BE->feed = BSide->Y;
        channels[bit]->DIR->feed = DIR;
        channels[bit]->A->OE_->feed = OE_;
        channels[bit]->B->OE_->feed = OE_;
        A[bit] = channels[bit]->A->O;
        B[bit] = channels[bit]->B->O;
    }
}

struct ChannelView : public Package<4> {
    explicit ChannelView(Vector2 pin1)
        : Package<4>(pin1)
    {
        rect = { pin1_tx.x, pin1_tx.y, 6 * PITCH, 6 * PITCH };
    }

    void render() override
    {
        auto p = pin1_tx;
        DrawRectangleLines(p.x + PITCH, p.y + PITCH, 4 * PITCH, 4 * PITCH, BLACK);
        DrawTriangleLines(
            { p.x + PITCH, p.y + 3.4f * PITCH },
            { p.x + PITCH, p.y + 4.6f * PITCH },
            { p.x + 2 * PITCH, p.y + 4 * PITCH },
            BLACK);
        DrawCircleV({ p.x + 2 * PITCH, p.y + PITCH }, 0.4f * PITCH, pin_color(pins[0]));
        DrawCircleV({ p.x + 4 * PITCH, p.y + PITCH }, 0.4f * PITCH, pin_color(pins[1]));
        DrawCircleV({ p.x + PITCH, p.y + 3 * PITCH }, 0.4f * PITCH, pin_color(pins[2]));
        DrawCircleV({ p.x + 5 * PITCH, p.y + 3 * PITCH }, 0.4f * PITCH, pin_color(pins[3]));
    }
};

template<>
void connect(LS245::Channel *device, ChannelView *package)
{
    package->pins[0] = device->AE;
    package->pins[1] = device->BE;
    package->pins[2] = device->A->O;
    package->pins[3] = device->B->O;
}

void LS245_channel_test(Board &board)
{
    auto  inputs = std::array<Pin *, 2> {};
    auto *DIR = board.circuit.add_component<TieDown>(PinState::High);
    inputs[0] = DIR->Y;
    auto *OE_ = board.circuit.add_component<TieDown>(PinState::High);
    inputs[1] = OE_->Y;
    auto *S = board.add_package<DIPSwitch<2, Orientation::North>>(Vector2 { 10, 1 });
    connect(inputs, S);

    auto *OEinv = board.circuit.add_component<Inverter>();
    board.add_device<Inverter, InverterIcon>(OEinv, Vector2 { 1, 5 });
    auto *ASide = board.circuit.add_component<AndGate>();
    board.add_device<AndGate, AndIcon>(ASide, Vector2 { 5, 5 });
    auto *BSide = board.circuit.add_component<NorGate>();
    board.add_device<NorGate, NorIcon>(BSide, Vector2 { 10, 5 });
    OEinv->A->feed = OE_->Y;
    ASide->A1->feed = DIR->Y;
    ASide->A2->feed = OEinv->Y;
    BSide->A1->feed = DIR->Y;
    BSide->A2->feed = OE_->Y;

    auto *A = board.circuit.add_component<TieDown>(PinState::Z);
    auto *Sw_A = board.add_package<TriStateSwitch<1, Orientation::North>>(Vector2 { 5, 10 });
    connect(A->Y, Sw_A);

    auto *B = board.circuit.add_component<TieDown>(PinState::Z);
    auto *Sw_B = board.add_package<TriStateSwitch<1, Orientation::North>>(Vector2 { 15, 10 });
    connect(B->Y, Sw_B);

    auto *channel = board.circuit.add_component<LS245::Channel>();
    channel->AE->feed = ASide->Y;
    channel->BE->feed = BSide->Y;
    channel->A->O->feed = A->Y;
    channel->B->O->feed = B->Y;
    channel->DIR->feed = DIR->Y;

    board.add_device<TriStateBuffer, TriStateIcon>(channel->Abuf, Vector2 { 5, 14});
    board.add_device<TriStateBuffer, TriStateIcon>(channel->Bbuf, Vector2 { 15, 14});

    auto *L_A = board.add_package<LEDArray<1, Orientation::North>>(Vector2 { 6, 18 });
    connect(channel->A->O, L_A);
    auto *L_B = board.add_package<LEDArray<1, Orientation::North>>(Vector2 { 16, 18 });
    connect(channel->B->O, L_B);
}

void LS245_test(Board &board)
{
    board.circuit.name = "LS245 Test";
    auto *ls245 = board.circuit.add_component<LS245>();
    board.add_device<LS245, DIP<20, Orientation::North>>(ls245, Vector2 { 3, 29 });

    auto  inputs = std::array<Pin *, 2> {};
    auto *DIR = board.circuit.add_component<TieDown>(PinState::High);
    inputs[0] = DIR->Y;
    auto *OE_ = board.circuit.add_component<TieDown>(PinState::High);
    inputs[1] = OE_->Y;
    auto *S = board.add_package<DIPSwitch<2, Orientation::North>>(Vector2 { 6, 1 });
    connect(inputs, S);
    board.add_text({1,1}, "DIR");
    board.add_text({13, 1}, "H: A->B, L: B->A");
    board.add_text({1, 3}, "OE_");

    ls245->DIR->feed = DIR->Y;
    ls245->OE_->feed = OE_->Y;

    auto a_switches = std::array<Pin *, 8> {};
    auto b_switches = std::array<Pin *, 8> {};
    for (auto bit = 0; bit < 8; ++bit) {
        {
            auto *tiedown = board.circuit.add_component<TieDown>(PinState::Z);
            a_switches[bit] = tiedown->Y;
            tiedown->Y->feed = ls245->A[bit];
            ls245->A[bit]->feed = tiedown->Y;
        }
        {
            auto *tiedown = board.circuit.add_component<TieDown>(PinState::Z);
            b_switches[bit] = tiedown->Y;
            tiedown->Y->feed = ls245->B[bit];
            ls245->B[bit]->feed = tiedown->Y;
        }
        board.add_device<LS245::Channel, ChannelView>(ls245->channels[bit], Vector2 { 13, 9.0f + bit * 8 });
    }
    auto *A_sw = board.add_package<TriStateSwitch<8, Orientation::North>>(Vector2 { 1, 9 });
    connect(a_switches, A_sw);
    board.add_text({1, 6}, "A0-A7");
    auto *B_sw = board.add_package<TriStateSwitch<8, Orientation::North>>(Vector2 { 23, 9 });
    board.add_text({23, 6}, "B0-B7");
    connect(b_switches, B_sw);
}

}
