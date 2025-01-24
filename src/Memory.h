/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Device.h"
#include "Graphics.h"
#include "LogicGate.h"

namespace Simul {

template<uint8_t X, uint8_t Y>
concept less_than = X < Y;

enum class MemoryIC {
    EEPROM_28C256,
    SRAM_LY62256,
};

inline char const *MemoryIC_name(MemoryIC memory_ic)
{
    switch (memory_ic) {
    case MemoryIC::EEPROM_28C256:
        return "28C256";
    case MemoryIC::SRAM_LY62256:
        return "LY62256";
    }
}

template<MemoryIC Type, uint8_t AddressBits, bool Writable = true>
    requires less_than<AddressBits, 17>
struct Memory : public Device {
    std::array<uint8_t, 1 << AddressBits> bytes {};
    std::array<Pin *, 8>                  D {};
    std::array<Pin *, AddressBits>        A {};
    std::array<TriStateBuffer *, 8>       buffers;
    std::array<Pin *, 8>                  I;
    Pin                                  *CE_ { nullptr };
    Pin                                  *WE_ { nullptr };
    Pin                                  *OE_ { nullptr };

    Memory()
        : Device(MemoryIC_name(Type))
    {
        auto *OEinv = add_component<Inverter>();
        OE_ = OEinv->A;
        auto *OE = OEinv->Y;
        for (auto bit = 0; bit < 8; ++bit) {
            buffers[bit] = add_component<TriStateBuffer>();
            buffers[bit]->E->feed = OE;
            I[bit] = buffers[bit]->A;
            D[bit] = buffers[bit]->Y;
        }
        for (auto bit = 0; bit < AddressBits; ++bit) {
            A[bit] = add_pin(9 + bit, std::format("A{}", bit), PinState::Low);
        }
        CE_ = add_pin(25, "CE_", PinState::High);
        WE_ = add_pin(25, "WE_", PinState::High);
        simulate_device = [this](Device *, duration) -> void {
            if (CE_->on()) {
                for (auto bit = 0; bit < 8; ++bit) {
                    D[bit]->new_driving = false;
                }
                return;
            }
            uint16_t addr = get_pins<AddressBits, uint16_t>(A);
            if (WE_->off() && Writable) {
                auto value = get_pins(D);
                bytes[addr] = value;
            }
            if (OE_->off()) {
                set_pins(I, bytes[addr]);
            }
        };
    }
};

using EEPROM_28C256 = Memory<MemoryIC::EEPROM_28C256, 15, false>;
using SRAM_LY62256 = Memory<MemoryIC::SRAM_LY62256, 15, true>;

template<Orientation O>
inline void connect(EEPROM_28C256 *device, DIP<28, O> *package)
{
    assign_pins<8, 8, 28, 0, 11>(device->D, package->pins);
    package->pins[9] = device->A[0];
    package->pins[8] = device->A[1];
    package->pins[7] = device->A[2];
    package->pins[6] = device->A[3];
    package->pins[5] = device->A[4];
    package->pins[4] = device->A[5];
    package->pins[3] = device->A[6];
    package->pins[2] = device->A[7];
    package->pins[24] = device->A[8];
    package->pins[23] = device->A[9];
    package->pins[20] = device->A[10];
    package->pins[22] = device->A[11];
    package->pins[1] = device->A[12];
    package->pins[25] = device->A[13];
    package->pins[0] = device->A[14];
    package->pins[21] = device->OE_;
    package->pins[26] = device->WE_;
    package->pins[19] = device->CE_;
}

template<Orientation O>
inline void connect(SRAM_LY62256 *device, DIP<28, O> *package)
{
    assign_pins<8, 8, 28, 0, 11>(device->D, package->pins);
    package->pins[9] = device->A[0];
    package->pins[8] = device->A[1];
    package->pins[7] = device->A[2];
    package->pins[6] = device->A[3];
    package->pins[5] = device->A[4];
    package->pins[4] = device->A[5];
    package->pins[3] = device->A[6];
    package->pins[2] = device->A[7];
    package->pins[24] = device->A[8];
    package->pins[23] = device->A[9];
    package->pins[20] = device->A[10];
    package->pins[22] = device->A[11];
    package->pins[1] = device->A[12];
    package->pins[25] = device->A[13];
    package->pins[0] = device->A[14];
    package->pins[21] = device->OE_;
    package->pins[26] = device->WE_;
    package->pins[19] = device->CE_;
}

void memory_test(Board &board);

}
