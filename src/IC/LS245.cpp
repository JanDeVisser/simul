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
    Abuf = add_component<TriStateBuffer>();
    Bbuf = add_component<TriStateBuffer>();
    A = Abuf->A;
    B = Bbuf->A;
    auto *DirInv = add_component<Inverter>();
    DIR = DirInv->A;
    AE = Abuf->E;
    BE = Bbuf->E;
    simulate_device = [this](Device *, duration) -> void {
        if (AE->on()) {
            B->new_state = Abuf->Y->new_state;
        }
        if (BE->on()) {
            A->new_state = Bbuf->Y->new_state;
        }
    };
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
    BSide->A1->feed = DIR;
    BSide->A2->feed = OE_;
    for (auto bit = 0; bit < 8; ++bit) {
        channels[bit] = add_component<Channel>();
        channels[bit]->AE->feed = ASide->Y;
        channels[bit]->BE->feed = BSide->Y;
        channels[bit]->DIR->feed = DIR;
        A[bit] = channels[bit]->A;
        B[bit] = channels[bit]->B;
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
        auto p { pin1_tx };
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
    package->pins[2] = device->A;
    package->pins[3] = device->B;
}

void LS245_channel_test(Board &board)
{
    auto  inputs = std::array<Pin *, 2> {};
    auto *DIR = board.circuit.add_component<TieDown>(PinState::High);
    inputs[0] = DIR->Y;
    auto *OE_ = board.circuit.add_component<TieDown>(PinState::High);
    inputs[1] = OE_->Y;
    auto *S = board.add_package<DIPSwitch<2, Orientation::North>>(10, 1);
    connect(inputs, S);

    auto *OEinv = board.add_device<Inverter, InverterIcon>(1, 5);
    auto *ASide = board.add_device<AndGate, AndIcon>(5, 5);
    auto *BSide = board.add_device<NorGate, NorIcon>(10, 5);
    OEinv->A->feed = OE_->Y;
    ASide->A1->feed = DIR->Y;
    ASide->A2->feed = OEinv->Y;
    BSide->A1->feed = DIR->Y;
    BSide->A2->feed = OE_->Y;

    auto *A = board.circuit.add_component<TieDown>(PinState::Z);
    auto *Sw_A = board.add_package<TriStateSwitch<1, Orientation::North>>(5, 10);
    connect(A->Y, Sw_A);

    auto *B = board.circuit.add_component<TieDown>(PinState::Z);
    auto *Sw_B = board.add_package<TriStateSwitch<1, Orientation::North>>(15, 10);
    connect(B->Y, Sw_B);

    auto *channel = board.circuit.add_component<LS245::Channel>();
    channel->AE->feed = ASide->Y;
    channel->BE->feed = BSide->Y;
    channel->A->feed = A->Y;
    channel->B->feed = B->Y;
    channel->DIR->feed = DIR->Y;

    board.add_device<TriStateBuffer, TriStateIcon>(channel->Abuf, 5, 14);
    board.add_device<TriStateBuffer, TriStateIcon>(channel->Bbuf, 15, 14);

    auto *L_A = board.add_package<LEDArray<1, Orientation::North>>(6, 18);
    connect(channel->A, L_A);
    auto *L_B = board.add_package<LEDArray<1, Orientation::North>>(16, 18);
    connect(channel->B, L_B);
}

void LS245_test(Board &board)
{
    board.circuit.name = "LS245 Test";
    auto *ls245 = board.add_device<LS245, DIP<20, Orientation::North>>(3, 29);

    auto  inputs = std::array<Pin *, 2> {};
    auto *DIR = board.circuit.add_component<TieDown>(PinState::High);
    inputs[0] = DIR->Y;
    auto *OE_ = board.circuit.add_component<TieDown>(PinState::High);
    inputs[1] = OE_->Y;
    auto *S = board.add_package<DIPSwitch<2, Orientation::North>>(6, 1);
    connect(inputs, S);
    board.add_text(1, 1, "DIR");
    board.add_text(13, 1, "H: A->B, L: B->A");
    board.add_text(1, 3, "OE_");

    ls245->DIR->feed = DIR->Y;
    ls245->OE_->feed = OE_->Y;

    for (auto bit = 0; bit < 8; ++bit) {
        board.add_device<LS245::Channel, ChannelView>(ls245->channels[bit], 13, 9 + bit * 8);
    }
    auto *A_sw = board.add_package<TriStateSwitch<8, Orientation::North>>(1, 9);
    connect(ls245->A, A_sw);
    board.add_text(1, 6, "A0-A7");
    auto *B_sw = board.add_package<TriStateSwitch<8, Orientation::North>>(23, 9);
    board.add_text(23, 6, "B0-B7");
    connect(ls245->B, B_sw);
}

}
