/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <thread>
#include <vector>

#include <raylib.h>

#include "Circuit/Graphics.h"
#include "Circuit/Latch.h"
#include "Circuit/Memory.h"
#include "Circuit/Oscillator.h"
#include "Circuit/UtilityDevice.h"

#include "IC/LS157.h"
#include "IC/LS193.h"
#include "IC/LS245.h"
#include "IC/LS377.h"
#include "IC/LS382.h"

namespace Simul {

template<typename D>
void test_board(Board &board)
{
    static_assert(false, "Implement test_board!");
}

template<>
void test_board<SRLatch>(Board &board)
{
    board.circuit.name = "SR-Latch Test";
    auto *latch = board.circuit.add_component<SRLatch>();
    auto *S_ = board.circuit.add_component<TieDown>(PinState::High);
    board.add_text(1, 1, "S_");
    auto *R_ = board.circuit.add_component<TieDown>(PinState::High);
    board.add_text(1, 3, "R_");
    latch->S_->feed = S_->Y;
    latch->R_->feed = R_->Y;
    auto S = board.add_package<DIPSwitch<2, Orientation::North>>(5, 1);
    connect(std::array<Pin *, 2> { S_->Y, R_->Y }, S);
    auto L = board.add_package<LEDArray<2, Orientation::North>>(13, 1);
    connect(std::array<Pin *, 2> { latch->Q, latch->Q_ }, L);
}

template<size_t Inputs>
void test_GatedSRLatch(Board &board)
{
    board.circuit.name = "Gated SR-Latch Test";
    auto                                 *latch { board.circuit.add_component<GatedSRLatch<Inputs>>() };
    std::array<TieDown *, 2 * Inputs + 3> In {};
    for (auto input = 0; input < Inputs; ++input) {
        In[input] = board.circuit.add_component<TieDown>(PinState::Low);
        latch->S_[input]->feed = In[input]->Y;
        board.add_text(1, 1 + 2 * input, std::format("S_{}", input));
        In[Inputs + input] = board.circuit.add_component<TieDown>(PinState::Low);
        latch->R_[input]->feed = In[Inputs + input]->Y;
        board.add_text(1, 1 + 2 * Inputs + 2 * input, std::format("R_{}", input));
    }
    In[2 * Inputs] = board.circuit.add_component<TieDown>(PinState::Low);
    latch->E->feed = In[2 * Inputs]->Y;
    board.add_text(1, 1 + 4 * Inputs, "E");
    In[2 * Inputs + 1] = board.circuit.add_component<TieDown>(PinState::High);
    latch->SET_->feed = In[2 * Inputs + 1]->Y;
    board.add_text(1, 3 + 4 * Inputs, "SET_");
    In[2 * Inputs + 2] = board.circuit.add_component<TieDown>(PinState::High);
    latch->CLR_->feed = In[2 * Inputs + 2]->Y;
    board.add_text(1, 5 + 4 * Inputs, "CLR_");

    auto S = board.add_package<DIPSwitch<2 * Inputs + 3, Orientation::North>>(7, 1);
    connect(In, S);
    auto L = board.add_package<LEDArray<2, Orientation::North>>(14, 1);
    connect(std::array<Pin *, 2> { latch->Q, latch->Q_ }, L);
}

template<>
void test_board<GatedSRLatch<1>>(Board &board)
{
    test_GatedSRLatch<1>(board);
}

template<>
void test_board<GatedSRLatch<2>>(Board &board)
{
    test_GatedSRLatch<2>(board);
}

template<>
void test_board<GatedSRLatch<3>>(Board &board)
{
    test_GatedSRLatch<3>(board);
}

template<>
void test_board<GatedSRLatch<4>>(Board &board)
{
    test_GatedSRLatch<4>(board);
}

template<>
void test_board<DFlipFlop>(Board &board)
{
    board.circuit.name = "D-Flip Flop Test";
    auto *latch = board.add_device<DFlipFlop, DFlipFlopIcon>(10, 3);
    auto *Clk = board.circuit.add_component<TieDown>(PinState::Low);
    board.add_text(1, 1, "CLK");
    auto *D = board.circuit.add_component<TieDown>(PinState::Low);
    board.add_text(1, 3, "D");
    auto *Set = board.circuit.add_component<TieDown>(PinState::High);
    board.add_text(1, 5, "SET_");
    auto *Clr = board.circuit.add_component<TieDown>(PinState::High);
    board.add_text(1, 7, "CLR_");
    latch->D->feed = D->Y;
    latch->CLR_->feed = Clr->Y;
    latch->SET_->feed = Set->Y;
    latch->CLK->feed = Clk->Y;
    auto S = board.add_package<DIPSwitch<4, Orientation::North>>(5, 1);
    connect(std::array<Pin *, 4> { Clk->Y, D->Y, Set->Y, Clr->Y }, S);
    auto L = board.add_package<LEDArray<2, Orientation::North>>(18, 2);
    connect(std::array<Pin *, 2> { latch->Q, latch->Q_ }, L);

    //    board.add_device<NandGate, NandIcon>(latch->J_gate, 10, 10);
    //    board.add_device<NandGate, NandIcon>(latch->K_gate, 10, 20);
    //    board.add_device<AndGate, AndIcon>(latch->set, 16, 10);
    //    board.add_device<AndGate, AndIcon>(latch->clr, 16, 20);
}

template<>
void test_board<JKFlipFlop>(Board &board)
{
    board.circuit.name = "JK-Flip Flop Test";
    auto *latch = board.add_device<JKFlipFlop, JKFlipFlopIcon>(10, 3);
    auto *Clk = board.circuit.add_component<TieDown>(PinState::Low);
    board.add_text(1, 1, "CLK");
    auto *J = board.circuit.add_component<TieDown>(PinState::Low);
    board.add_text(1, 3, "J");
    auto *K = board.circuit.add_component<TieDown>(PinState::Low);
    board.add_text(1, 5, "K");
    auto *Set = board.circuit.add_component<TieDown>(PinState::High);
    board.add_text(1, 7, "SET_");
    auto *Clr = board.circuit.add_component<TieDown>(PinState::High);
    board.add_text(1, 9, "CLR_");
    latch->J->feed = J->Y;
    latch->K->feed = K->Y;
    latch->CLR_->feed = Clr->Y;
    latch->SET_->feed = Set->Y;
    latch->CLK->feed = Clk->Y;
    auto S = board.add_package<DIPSwitch<5, Orientation::North>>(5, 1);
    connect(std::array<Pin *, 5> { Clk->Y, J->Y, K->Y, Set->Y, Clr->Y }, S);
    auto L = board.add_package<LEDArray<2, Orientation::North>>(18, 2);
    connect(std::array<Pin *, 2> { latch->Q, latch->Q_ }, L);

    board.add_device<NandGate, NandIcon>(latch->J_gate, 10, 10);
    board.add_device<NandGate, NandIcon>(latch->K_gate, 10, 20);
    board.add_device<AndGate, AndIcon>(latch->set, 16, 10);
    board.add_device<AndGate, AndIcon>(latch->clr, 16, 20);
}

template<>
void test_board<TFlipFlop>(Board &board)
{
    board.circuit.name = "T-Flip Flop Test";
    auto *latch = board.add_device<TFlipFlop, TFlipFlopIcon>(9, 1);
    auto *Clk = board.circuit.add_component<TieDown>(PinState::Low);
    board.add_text(1, 1, "CLK");
    auto *Set = board.circuit.add_component<TieDown>(PinState::High);
    board.add_text(1, 3, "Set_");
    auto *Clr = board.circuit.add_component<TieDown>(PinState::High);
    board.add_text(1, 5, "Clr_");
    latch->T->feed = board.circuit.VCC;
    latch->CLK->feed = Clk->Y;
    latch->CLR_->feed = Clr->Y;
    latch->SET_->feed = Set->Y;
    auto S = board.add_package<DIPSwitch<3, Orientation::North>>(5, 1);
    connect(std::array<Pin *, 3> { Clk->Y, Set->Y, Clr->Y }, S);
    auto L = board.add_package<LEDArray<2, Orientation::North>>(16, 2);
    connect(std::array<Pin *, 2> { latch->Q, latch->Q_ }, L);
}

template<>
void test_board<LS193>(Board &board)
{
    board.circuit.name = "LS377 Test";
    auto *ic = board.circuit.add_component<LS193>();
    board.add_device<LS193, DIP<16, Orientation::North>>(ic, 15, 4);

    auto  controls = std::array<Pin *, 4> {};
    auto *UP = board.circuit.add_component<TieDown>(PinState::Low);
    controls[0] = UP->Y;
    board.add_text(1, 1, "Up");
    auto *DOWN = board.circuit.add_component<TieDown>(PinState::Low);
    controls[1] = DOWN->Y;
    board.add_text(1, 3, "Down");
    auto *CLR = board.circuit.add_component<TieDown>(PinState::Low);
    controls[2] = CLR->Y;
    board.add_text(1, 5, "CLR");
    auto *LOAD = board.circuit.add_component<TieDown>(PinState::High);
    controls[3] = LOAD->Y;
    board.add_text(1, 7, "LOAD_");
    auto *S = board.add_package<DIPSwitch<4, Orientation::North>>(8, 1);
    connect(controls, S);

    ic->Up->feed = UP->Y;
    ic->Down->feed = DOWN->Y;
    ic->Load_->feed = LOAD->Y;
    ic->CLR->feed = CLR->Y;

    auto d_switches = std::array<Pin *, 4> {};
    auto d_pins = std::array<Pin *, 4> {};
    for (auto bit = 0; bit < 4; ++bit) {
        auto *tiedown = board.circuit.add_component<TieDown>(PinState::Low);
        d_switches[bit] = tiedown->Y;
        ic->D[bit]->feed = tiedown->Y;
        d_pins[bit] = tiedown->Y;
        board.add_text(1, 12 + (2 * bit), std::format("D{}", bit));
        board.add_text(25, 7 + (2 * bit), std::format("Q{}", bit));
    }
    auto *data_S = board.add_package<DIPSwitch<4, Orientation::North>>(8, 12);
    connect(d_pins, data_S);
    auto *L = board.add_package<LEDArray<4, Orientation::North>>(23, 7);
    connect(ic->Q, L);
    auto *L2 = board.add_package<LEDArray<2, Orientation::North>>(23, 15);
    board.add_text(25, 15, "BO_");
    board.add_text(25, 17, "CO_");
    connect(std::array<Pin *, 2> { ic->BO_, ic->CO_ }, L2);

    board.add_text(1, 21, "SetNand");
    board.add_text(1, 23, "ResetNand");
    board.add_text(1, 25, "ClockOr");
    board.add_text(1, 27, "ResetOr");
    for (auto bit = 0; bit < 4; ++bit) {
        auto *latch = ic->latches[bit];
        //        board.add_device<TFlipFlop, TFlipFlopIcon>(latch->latch, 25, 5);

        auto *gates = board.add_package<LEDArray<4, Orientation::North>>(12 + bit * 6, 21);
        connect(std::array<Pin *, 4> {
                    latch->SetNand->Y,
                    latch->LoadNand->Y,
                    latch->ClockOr->Y,
                    latch->ResetOr->Y,
                },
            gates);
    }
}

template<>
void test_board<LS193_Bit0>(Board &board)
{
    board.circuit.name = "LS377 Bit 0 Test";
    auto *ic = board.circuit.add_component<LS193_Bit0>();

    auto  controls = std::array<Pin *, 5> {};
    auto *UP = board.circuit.add_component<TieDown>(PinState::Low);
    controls[0] = UP->Y;
    board.add_text(1, 1, "Up");
    auto *DOWN = board.circuit.add_component<TieDown>(PinState::Low);
    controls[1] = DOWN->Y;
    board.add_text(1, 3, "Down");
    auto *CLR = board.circuit.add_component<TieDown>(PinState::Low);
    controls[2] = CLR->Y;
    board.add_text(1, 5, "CLR");
    auto *LOAD = board.circuit.add_component<TieDown>(PinState::High);
    controls[3] = LOAD->Y;
    board.add_text(1, 7, "LOAD_");
    auto *D = board.circuit.add_component<TieDown>(PinState::High);
    controls[4] = D->Y;
    board.add_text(3, 9, "D");
    auto *S = board.add_package<DIPSwitch<5, Orientation::North>>(7, 1);
    connect(controls, S);

    ic->Up->feed = UP->Y;
    ic->Down->feed = DOWN->Y;
    ic->Load_->feed = LOAD->Y;
    ic->CLR->feed = CLR->Y;
    ic->D->feed = D->Y;

    board.add_device<TFlipFlop, TFlipFlopIcon>(ic->latch, 25, 5);

    board.add_text(18, 1, "SetNand");
    board.add_text(18, 3, "ResetNand");
    board.add_text(18, 5, "ClockOr");
    board.add_text(18, 7, "ResetOr");
    auto *gates = board.add_package<LEDArray<4, Orientation::North>>(16, 1);
    connect(std::array<Pin *, 4> {
                ic->SetNand->Y,
                ic->LoadNand->Y,
                ic->ClockOr->Y,
                ic->ResetOr->Y,
            },
        gates);

    board.add_text(34, 1, "Set_");
    board.add_text(34, 3, "Clr_");
    board.add_text(34, 5, "Q");
    board.add_text(34, 7, "BO_");
    board.add_text(34, 9, "CO_");
    auto *L = board.add_package<LEDArray<5, Orientation::North>>(32, 1);
    connect(std::array<Pin *, 5> {
                ic->latch->SET_,
                ic->latch->CLR_,
                ic->Q,
                ic->BO_,
                ic->CO_ },
        L);
}

void main(int argc, char **argv)
{
    std::string device { "TFlipFlop" };
    if (argc > 1) {
        device = argv[1];
    }
    InitWindow(30 * static_cast<int>(PITCH), 30 * static_cast<int>(PITCH), "Simul");
    {
        SetWindowState(FLAG_VSYNC_HINT);
        SetTargetFPS(60);
        auto font = LoadFontEx("fonts/Tecnico-Bold.ttf", 15, nullptr, 0);
        {
            auto &circuit = Circuit::the();
            Board board { circuit, font };
            if (device == "LS193") {
                test_board<LS193>(board);
            } else if (device == "SRLatch") {
                test_board<SRLatch>(board);
            } else if (device == "DFlipFlop") {
                test_board<DFlipFlop>(board);
            } else if (device == "GatedSRLatch") {
                size_t gates = 1;
                if (argc > 2) {
                    gates = strtoul(argv[2], nullptr, 10);
                    if (gates == 0 || gates > 4) {
                        gates = 1;
                    }
                }
                switch (gates) {
                case 2:
                    test_board<GatedSRLatch<2>>(board);
                    break;
                case 3:
                    test_board<GatedSRLatch<3>>(board);
                    break;
                case 4:
                    test_board<GatedSRLatch<4>>(board);
                    break;
                default:
                    test_board<GatedSRLatch<1>>(board);
                    break;
                }
            } else if (device == "LS157") {
                LS157_test(board);
            } else if (device == "LS193_Bit0") {
                test_board<LS193_Bit0>(board);
            } else if (device == "LS245") {
                LS245_test(board);
            } else if (device == "LS377") {
                LS377_test(board);
            } else if (device == "LS377_Latch") {
                LS377_latch_test(board);
            } else if (device == "LS382_decoder") {
                LS382_decoder_test(board);
            } else if (device == "LS382") {
                LS382_test(board);
            } else if (device == "TFlipFlop") {
                test_board<TFlipFlop>(board);
            } else if (device == "SRAM") {
                memory_test(board);
            } else {
                test_board<JKFlipFlop>(board);
            }
            board.layout(0, 0, board.size.x, board.size.y);
            SetWindowSize(static_cast<int>(board.size.x), static_cast<int>(board.size.y));
            auto t = circuit.start_simulation();
            while (!WindowShouldClose()) {
                board.handle_input();
                BeginDrawing();
                board.render();
                EndDrawing();
            }
            circuit.stop();
            t.join();
        }
        UnloadFont(font);
    }
    CloseWindow();
}

}

int main(int argc, char **argv)
{
    Simul::main(argc, argv);
    return 0;
}
