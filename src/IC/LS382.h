/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-reserved-identifier"

#include "Device.h"
#include "Graphics.h"
#include "LogicGate.h"
#include "Pin.h"

namespace Simul {
struct LS382 : public Device {
    struct FunctionDecoder : public Device {
        std::array<Pin *, 3>      S {};
        std::array<Pin *, 3>      S_ {};
        std::array<Pin *, 7>      D {};
        std::array<Inverter *, 3> inverters {};
        std::array<AndGate *, 3>  D0_gates {};
        NorGate                  *D0_result;
        std::array<AndGate *, 3>  D1_gates {};
        NorGate                  *D1_result;
        std::array<AndGate *, 2>  D2_gates {};
        NorGate                  *D2_result;
        NandGate                 *D3_result;
        NandGate                 *D4_result;
        NandGate                 *D5_result;
        std::array<AndGate *, 2>  D6_gates {};
        OrGate                   *D6_result;

        FunctionDecoder();
    };

    struct InputChannel : public Device {
        Pin             *A;
        Pin             *A_;
        Pin             *B;
        Pin             *B_;
        Pin             *Aout;
        Pin             *Bout;
        Inverter        *Ainv;
        Inverter        *Binv;
        AndGate         *AB__1;
        AndGate         *AB_1;
        AndGate         *A_B_1;
        AndGate         *A_B__1;
        AndGate         *AB__2;
        AndGate         *AB_2;
        AndGate         *A_B_2;
        AndGate         *A_B__2;
        NorGate         *combine_1;
        NorGate         *combine_2;
        FunctionDecoder *decoder;

        explicit InputChannel(FunctionDecoder *decoder);
    };

    struct F0_adder : public Device {
        NandGate *adder;
        XNorGate *out;
        Pin      *F;

        explicit F0_adder(LS382 *ic);
    };

    struct F1_adder : public Device {
        AndGate  *adder_1;
        AndGate  *adder_2;
        NorGate  *combine;
        XNorGate *out;
        Pin      *F;

        explicit F1_adder(LS382 *ic);
    };

    struct F2_adder : public Device {
        AndGate  *adder_1;
        AndGate  *adder_2;
        AndGate  *adder_3;
        NorGate  *combine;
        XNorGate *out;
        Pin      *F;

        explicit F2_adder(LS382 *ic);
    };

    struct F3_adder : public Device {
        AndGate  *adder_1;
        AndGate  *adder_2;
        AndGate  *adder_3;
        AndGate  *adder_4;
        NorGate  *combine;
        XNorGate *out;
        Pin      *F;

        explicit F3_adder(LS382 *ic);
    };

    struct Cout_adder : public Device {
        AndGate  *adder_1;
        AndGate  *adder_2;
        AndGate  *adder_3;
        AndGate  *adder_4;
        NorGate  *combine;
        Inverter *out;
        Pin      *Cout;

        explicit Cout_adder(LS382 *ic);
    };

    FunctionDecoder              *decoder;
    std::array<InputChannel *, 4> inputs {};
    F0_adder                     *f0_adder;
    F1_adder                     *f1_adder;
    F2_adder                     *f2_adder;
    F3_adder                     *f3_adder;
    Cout_adder                   *cout_adder;
    XorGate                      *OVR_gate;
    std::array<Pin *, 3>          S {};
    Pin                          *Cin;
    Pin                          *Cout;
    Pin                          *OVR;
    std::array<Pin *, 7>          D {};
    std::array<Pin *, 4>          A {};
    std::array<Pin *, 4>          Aout {};
    std::array<Pin *, 4>          B {};
    std::array<Pin *, 4>          Bout {};
    std::array<Pin *, 4>          F {};

    LS382();
};

template<Orientation O>
inline void connect(LS382 *device, DIP<20, O> *package)
{
    package->pins[0] = device->A[1];
    package->pins[1] = device->B[1];
    package->pins[2] = device->A[0];
    package->pins[3] = device->B[0];
    package->pins[4] = device->S[0];
    package->pins[5] = device->S[1];
    package->pins[6] = device->S[2];
    package->pins[7] = device->F[0];
    package->pins[8] = device->F[1];
    package->pins[10] = device->F[2];
    package->pins[11] = device->F[3];
    package->pins[12] = device->OVR;
    package->pins[13] = device->Cout;
    package->pins[14] = device->Cin;
    package->pins[15] = device->B[3];
    package->pins[16] = device->A[3];
    package->pins[17] = device->B[2];
    package->pins[18] = device->A[2];
}

void LS382_test(Board &board);
void LS382_decoder_test(Board &board);

}

#pragma clang diagnostic pop
