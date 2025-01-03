/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Device.h"
#include "Graphics.h"

namespace Simul {

struct Inverter : public Device {
    Pin *A;
    Pin *Y;

    explicit Inverter(std::string const &ref = "");
    void simulate(duration) override;
};

struct InverterIcon : Package<2> {
    explicit InverterIcon(Vector2 pos);
    void render() override;
};

template<>
inline void connect(Inverter *device, InverterIcon *package)
{
    package->pins[0] = device->A;
    package->pins[1] = device->Y;
}

struct LogicGate : public Device {
    Pin *A1;
    Pin *A2;
    Pin *Y;

    explicit LogicGate(std::string const &name, int inputs = 2, std::string const &ref = "");
    void             simulate(duration) override;
    virtual PinState operate(PinState s1, PinState s2) = 0;
    virtual PinState finalize(PinState s);
};

struct LogicIcon : public Package<3>
{
    explicit LogicIcon(Vector2 pos);
    void render() override;
    [[nodiscard]] virtual char const * label() const = 0;
    [[nodiscard]] virtual bool neg() const { return false; }
};

template<class L, class Icon>
requires std::derived_from<L, LogicGate> && std::derived_from<Icon, LogicIcon>
inline void connect(L *device, Icon *package)
{
    package->pins[0] = device->A1;
    package->pins[1] = device->A2;
    package->pins[2] = device->Y;
}

struct AndGate : public LogicGate {
    explicit AndGate(int inputs = 2, std::string const &ref = "");
    PinState operate(PinState s1, PinState s2) override;

protected:
    explicit AndGate(std::string const &name, int inputs = 2, std::string const &ref = "");
};

struct AndIcon : public LogicIcon {
    explicit AndIcon(Vector2 pos);
    [[nodiscard]] char const * label() const override { return "&"; }
};

struct NandGate : public AndGate {
    explicit NandGate(int inputs = 2, std::string const &ref = "");
    PinState finalize(PinState s) override;
};

struct NandIcon : public AndIcon {
    explicit NandIcon(Vector2 pos);
    [[nodiscard]] virtual bool neg() const { return true; }
};

struct OrGate : public LogicGate {
    explicit OrGate(int inputs = 2, std::string const &ref = "");
    PinState operate(PinState s1, PinState s2) override;

protected:
    explicit OrGate(std::string const &name, int inputs = 2, std::string const &ref = "");
};

struct OrIcon : public LogicIcon {
    explicit OrIcon(Vector2 pos);
    [[nodiscard]] char const * label() const override { return ">=1"; }
};

struct NorGate : public OrGate {
    explicit NorGate(int inputs = 2, std::string const &ref = "");
    PinState finalize(PinState s) override;
};

struct NorIcon : public OrIcon {
    explicit NorIcon(Vector2 pos);
    [[nodiscard]] bool neg() const override { return true; }
};

struct XorGate : public LogicGate {
    explicit XorGate(std::string const &ref = "");
    PinState operate(PinState s1, PinState s2) override;
};

struct XorIcon : LogicIcon {
    explicit XorIcon(Vector2 pos);
    [[nodiscard]] char const * label() const override { return "=1"; }
};

struct TristatePin : public Device {
    Pin *I;
    Pin *O;
    Pin *DIR;
    Pin *OE_;

    explicit TristatePin(std::string const &ref = "");
    void simulate(duration) override;
};

struct TriStateBuffer : public Device {
    Pin *A;
    Pin *E;
    Pin *Y;

    explicit TriStateBuffer(std::string const &ref = "");
    void simulate(duration) override;
};

struct TriStateIcon : Package<3> {
    explicit TriStateIcon(Vector2 pos);
    void render() override;
};

template<>
inline void connect(TriStateBuffer *device, TriStateIcon *package)
{
    package->pins[0] = device->A;
    package->pins[1] = device->E;
    package->pins[2] = device->Y;
}

void LogicGate_test(Board &);

}
