/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "LS382.h"
#include "Circuit/UtilityDevice.h"

namespace Simul {

LS382::FunctionDecoder::FunctionDecoder()
    : Device("LS382 Function bit decoder")
{
    for (auto inv = 0; inv < 3; ++inv) {
        inverters[inv] = add_component<Inverter>();
        S[inv] = inverters[inv]->A;
        S_[inv] = inverters[inv]->Y;
    }

    D0_result = add_component<NorGate>(3);
    D[0] = D0_result->Y;
    for (auto ix = 0; ix < 3; ++ix) {
        D0_gates[ix] = add_component<AndGate>(3);
        D0_result->pins[ix]->feed = D0_gates[ix]->Y;
    }
    D0_gates[0]->pins[0]->feed = S_[2];
    D0_gates[0]->pins[1]->feed = S_[1];
    D0_gates[0]->pins[2]->feed = S[0];
    D0_gates[1]->pins[0]->feed = S_[2];
    D0_gates[1]->pins[1]->feed = S[1];
    D0_gates[1]->pins[2]->feed = S_[0];
    D0_gates[2]->pins[0]->feed = S[2];
    D0_gates[2]->pins[1]->feed = S[1];
    D0_gates[2]->pins[2]->feed = S[0];

    D1_result = add_component<NorGate>(3);
    D[1] = D1_result->Y;
    for (auto ix = 0; ix < 3; ++ix) {
        D1_gates[ix] = add_component<AndGate>(2);
        D1_result->pins[ix]->feed = D1_gates[ix]->Y;
    }
    D1_gates[0]->A1->feed = S_[1];
    D1_gates[0]->A2->feed = S[0];
    D1_gates[1]->A1->feed = S[2];
    D1_gates[1]->A2->feed = S[0];
    D1_gates[2]->A1->feed = S[1];
    D1_gates[2]->A2->feed = S_[0];

    D2_result = add_component<NorGate>(2);
    D[2] = D2_result->Y;
    for (auto ix = 0; ix < 2; ++ix) {
        D2_gates[ix] = add_component<AndGate>(2);
        D2_result->pins[ix]->feed = D2_gates[ix]->Y;
    }
    D2_gates[0]->A1->feed = S[1];
    D2_gates[0]->A2->feed = S[0];
    D2_gates[1]->A1->feed = S[2];
    D2_gates[1]->A2->feed = S_[1];

    D3_result = add_component<NandGate>(3);
    D[3] = D3_result->Y;
    D3_result->pins[0]->feed = S_[2];
    D3_result->pins[1]->feed = S_[1];
    D3_result->pins[2]->feed = S[0];

    D4_result = add_component<NandGate>(3);
    D[4] = D4_result->Y;
    D4_result->pins[0]->feed = S_[2];
    D4_result->pins[1]->feed = S[1];
    D4_result->pins[2]->feed = S[0];

    D5_result = add_component<NandGate>(3);
    D[5] = D5_result->Y;
    D5_result->pins[0]->feed = S_[2];
    D5_result->pins[1]->feed = S[1];
    D5_result->pins[2]->feed = S_[0];

    D6_result = add_component<OrGate>(2);
    D[6] = D6_result->Y;
    for (auto ix = 0; ix < 2; ++ix) {
        D6_gates[ix] = add_component<AndGate>(2);
        D6_result->pins[ix]->feed = D6_gates[ix]->Y;
    }
    D6_gates[0]->A1->feed = S_[2];
    D6_gates[0]->A2->feed = S[0];
    D6_gates[1]->A1->feed = S_[2];
    D6_gates[1]->A2->feed = S[1];
}

LS382::InputChannel::InputChannel(FunctionDecoder *decoder)
    : Device("LS382 Input Channel")
    , decoder(decoder)
{
    Ainv = add_component<Inverter>();
    Binv = add_component<Inverter>();
    A = Ainv->A;
    A_ = Ainv->Y;
    B = Binv->A;
    B_ = Binv->Y;

    AB__1 = add_component<AndGate>(3);
    AB__1->pins[0]->feed = A;
    AB__1->pins[1]->feed = B_;
    AB__1->pins[2]->feed = decoder->D[2];

    AB_1 = add_component<AndGate>(3);
    AB_1->pins[0]->feed = A;
    AB_1->pins[1]->feed = B;
    AB_1->pins[2]->feed = decoder->D[1];

    A_B_1 = add_component<AndGate>(3);
    A_B_1->pins[0]->feed = A_;
    A_B_1->pins[1]->feed = B;
    A_B_1->pins[2]->feed = decoder->D[2];

    A_B__1 = add_component<AndGate>(3);
    A_B__1->pins[0]->feed = A_;
    A_B__1->pins[1]->feed = B_;
    A_B__1->pins[2]->feed = decoder->D[0];

    combine_1 = add_component<NorGate>(4);
    combine_1->pins[0]->feed = AB__1->Y;
    combine_1->pins[1]->feed = AB_1->Y;
    combine_1->pins[2]->feed = A_B_1->Y;
    combine_1->pins[3]->feed = A_B__1->Y;
    Aout = combine_1->Y;

    AB__2 = add_component<AndGate>(3);
    AB__2->pins[0]->feed = A;
    AB__2->pins[1]->feed = B_;
    AB__2->pins[2]->feed = decoder->D[5];

    AB_2 = add_component<AndGate>(3);
    AB_2->pins[0]->feed = A;
    AB_2->pins[1]->feed = B;
    AB_2->pins[2]->feed = decoder->D[4];

    A_B_2 = add_component<AndGate>(3);
    A_B_2->pins[0]->feed = A_;
    A_B_2->pins[1]->feed = B;
    A_B_2->pins[2]->feed = decoder->D[3];

    A_B__2 = add_component<AndGate>();
    A_B__2->pins[0]->feed = A_;
    A_B__2->pins[1]->feed = B_;

    combine_2 = add_component<NorGate>(4);
    combine_2->pins[0]->feed = AB__2->Y;
    combine_2->pins[1]->feed = AB_2->Y;
    combine_2->pins[2]->feed = A_B_2->Y;
    combine_2->pins[3]->feed = A_B__2->Y;
    Bout = combine_2->Y;
}

LS382::F0_adder::F0_adder(LS382 *ic)
    : Device("LS382 F0 adder")
{
    adder = add_component<NandGate>();
    adder->A1->feed = ic->D[6];
    ic->Cin = adder->A2;
    out = add_component<XNorGate>();
    out->A1->feed = ic->Aout[0];
    out->A2->feed = adder->Y;
    F = out->Y;
}

LS382::F1_adder::F1_adder(LS382 *ic)
    : Device("LS382 F1 adder")
{
    adder_1 = add_component<AndGate>(3);
    adder_1->pins[0]->feed = ic->D[6];
    adder_1->pins[1]->feed = ic->Cin;
    adder_1->pins[2]->feed = ic->Aout[0];
    adder_2 = add_component<AndGate>();
    adder_2->A1->feed = ic->D[6];
    adder_2->A2->feed = ic->Bout[0];
    combine = add_component<NorGate>();
    combine->A1->feed = adder_1->Y;
    combine->A2->feed = adder_2->Y;
    out = add_component<XNorGate>();
    out->A1->feed = ic->Aout[1];
    out->A2->feed = combine->Y;
    F = out->Y;
}

LS382::F2_adder::F2_adder(LS382 *ic)
    : Device("LS382 F2 adder")
{
    adder_1 = add_component<AndGate>(4);
    adder_1->pins[0]->feed = ic->D[6];
    adder_1->pins[1]->feed = ic->Cin;
    adder_1->pins[2]->feed = ic->Aout[0];
    adder_1->pins[3]->feed = ic->Aout[1];
    adder_2 = add_component<AndGate>(3);
    adder_2->pins[0]->feed = ic->D[6];
    adder_2->pins[1]->feed = ic->Bout[0];
    adder_2->pins[2]->feed = ic->Aout[1];
    adder_3 = add_component<AndGate>(2);
    adder_3->A1->feed = ic->D[6];
    adder_3->A2->feed = ic->Bout[1];
    combine = add_component<NorGate>(3);
    combine->pins[0]->feed = adder_1->Y;
    combine->pins[1]->feed = adder_2->Y;
    combine->pins[2]->feed = adder_3->Y;
    out = add_component<XNorGate>();
    out->A1->feed = ic->Aout[2];
    out->A2->feed = combine->Y;
    F = out->Y;
}

LS382::F3_adder::F3_adder(LS382 *ic)
    : Device("LS382 F3 adder")
{
    adder_1 = add_component<AndGate>(5);
    adder_1->A1->feed = ic->D[6];
    adder_1->A2->feed = ic->Cin;
    adder_1->pins[2]->feed = ic->Aout[0];
    adder_1->pins[3]->feed = ic->Aout[1];
    adder_1->pins[4]->feed = ic->Aout[2];
    adder_2 = add_component<AndGate>(4);
    adder_2->A1->feed = ic->D[6];
    adder_2->A2->feed = ic->Bout[0];
    adder_2->pins[2]->feed = ic->Aout[1];
    adder_2->pins[3]->feed = ic->Aout[2];
    adder_3 = add_component<AndGate>(3);
    adder_3->A1->feed = ic->D[6];
    adder_3->A2->feed = ic->Bout[1];
    adder_3->pins[2]->feed = ic->Aout[2];
    adder_4 = add_component<AndGate>();
    adder_4->A1->feed = ic->D[6];
    adder_4->A2->feed = ic->Bout[2];
    combine = add_component<NorGate>(4);
    combine->A1->feed = adder_1->Y;
    combine->A2->feed = adder_2->Y;
    combine->pins[2]->feed = adder_3->Y;
    combine->pins[3]->feed = adder_4->Y;
    out = add_component<XNorGate>();
    out->A1->feed = ic->Aout[3];
    out->A2->feed = combine->Y;
    F = out->Y;
}

LS382::Cout_adder::Cout_adder(LS382 *ic)
    : Device("LS382 Carry Out adder")
{
    adder_1 = add_component<AndGate>(5);
    adder_1->pins[0]->feed = ic->Cin;
    adder_1->pins[1]->feed = ic->Aout[0];
    adder_1->pins[2]->feed = ic->Aout[1];
    adder_1->pins[3]->feed = ic->Aout[2];
    adder_1->pins[4]->feed = ic->Aout[3];
    adder_2 = add_component<AndGate>(4);
    adder_2->pins[0]->feed = ic->Bout[0];
    adder_2->pins[1]->feed = ic->Aout[1];
    adder_2->pins[2]->feed = ic->Aout[2];
    adder_2->pins[3]->feed = ic->Aout[3];
    adder_3 = add_component<AndGate>(3);
    adder_3->pins[0]->feed = ic->Bout[1];
    adder_3->pins[1]->feed = ic->Aout[2];
    adder_3->pins[2]->feed = ic->Aout[3];
    adder_4 = add_component<AndGate>();
    adder_4->A1->feed = ic->Bout[2];
    adder_4->A2->feed = ic->Aout[3];
    combine = add_component<NorGate>(5);
    combine->A1->feed = adder_1->Y;
    combine->A2->feed = adder_2->Y;
    combine->pins[2]->feed = adder_3->Y;
    combine->pins[3]->feed = adder_4->Y;
    combine->pins[4]->feed = ic->Bout[3];
    out = add_component<Inverter>();
    out->A->feed = combine->Y;
    Cout = out->Y;
}

LS382::LS382()
    : Device("LS328 - Arithmetic Logic Units/Function Generators")
{
    decoder = add_component<FunctionDecoder>();
    S = decoder->S;
    D = decoder->D;
    for (auto ix = 0; ix < 4; ++ix) {
        inputs[ix] = add_component<InputChannel>(decoder);
        A[ix] = inputs[ix]->A;
        Aout[ix] = inputs[ix]->Aout;
        B[ix] = inputs[ix]->B;
        Bout[ix] = inputs[ix]->Bout;
    }
    f0_adder = add_component<F0_adder>(this);
    F[0] = f0_adder->F;
    f1_adder = add_component<F1_adder>(this);
    F[1] = f1_adder->F;
    f2_adder = add_component<F2_adder>(this);
    F[2] = f2_adder->F;
    f3_adder = add_component<F3_adder>(this);
    F[3] = f3_adder->F;
    cout_adder = add_component<Cout_adder>(this);
    Cout = cout_adder->Cout;
    OVR_gate = add_component<XorGate>();
    OVR_gate->A1->feed = f3_adder->combine->Y;
    OVR_gate->A2->feed = cout_adder->combine->Y;
    OVR = OVR_gate->Y;
}

void LS382_test(Board &board)
{
    board.circuit.name = "LS382 Test";
    auto *ls382 = board.circuit.add_component<LS382>();
    board.add_device<LS382, DIP<20, Orientation::North>>(ls382, 17, 4);

    switches<3>(board, 10, 1, ls382->S);
    board.add_text(1, 1, "S0");
    board.add_text(1, 3, "S1");
    board.add_text(1, 5, "S2");

    switches<1>(board, 10, 8, { ls382->Cin });
    board.add_text(1, 8, "Cin");

    switches<4>(board, 10, 11, ls382->A);
    board.add_text(1, 11, "A0");
    board.add_text(1, 13, "A1");
    board.add_text(1, 15, "A2");
    board.add_text(1, 17, "A3");

    switches<4>(board, 10, 20, ls382->B);
    board.add_text(1, 20, "B0");
    board.add_text(1, 22, "B1");
    board.add_text(1, 24, "B2");
    board.add_text(1, 26, "B3");

    leds<4>(board, 27, 10, ls382->F);
    board.add_text(29, 10, "F0");
    board.add_text(29, 12, "F1");
    board.add_text(29, 14, "F2");
    board.add_text(29, 16, "F3");
    leds<2>(board, 27, 19, { ls382->OVR, ls382->Cout });
    board.add_text(29, 19, "OVR");
    board.add_text(29, 21, "Cout");

    leds<7>(board, 17, 26, ls382->D);
    leds<4>(board, 20, 26, ls382->Aout);
    leds<2>(board, 20, 35, {
                               ls382->Aout[0],
                               ls382->f0_adder->adder->Y,
                           });
    leds<2>(board, 23, 35, {
                               ls382->Aout[1],
                               ls382->f1_adder->combine->Y,
                           });
    leds<2>(board, 26, 35, {
                               ls382->Aout[1],
                               ls382->f2_adder->combine->Y,
                           });
    leds<3>(board, 29, 35, {
                               ls382->Aout[3],
                               ls382->f3_adder->combine->Y,
                               ls382->cout_adder->combine->Y,
                           });
    leds<4>(board, 23, 26, ls382->Bout);
}

void LS382_decoder_test(Board &board)
{
    board.circuit.name = "LS382 Decoder Test";
    auto *decoder = board.circuit.add_component<LS382::FunctionDecoder>();

    switches<3>(board, 6, 4, decoder->S);
    board.add_text(1, 4, "S0");
    board.add_text(1, 6, "S1");
    board.add_text(1, 8, "S2");

    leds<7>(board, 12, 1, decoder->D);
}

}
