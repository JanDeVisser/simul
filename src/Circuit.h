/*
 * Copyright (c) 2024, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <concepts>
#include <thread>

#include "Device.h"

namespace Simul {

struct Circuit : public Device {
    enum class SimStatus {
        Unstarted,
        Starting,
        Started,
        Stopping,
        Done,
    };

    std::array<Pin, 8192>   all_pins {};
    size_t                  pin_count { 0 };
    SimStatus               status { SimStatus::Unstarted };
    std::mutex              yield_mutex {};
    std::condition_variable yielder {};
    Pin                    *VCC { nullptr };
    Pin                    *GND { nullptr };

    void        initialize(std::string const &name = "");
    void        start();
    void        stop();
    void        done();
    std::thread start_simulation();
    void        yield();
    size_t      simulate(duration d);
    Pin        *allocate_pin(int nr, std::string const &pin_name, PinState state = PinState::Z);

    static Circuit &the();

private:
    Circuit()
        : Device("")
    {
        VCC = new (&all_pins[0]) Pin { -1, "VCC", PinState::High };
        GND = new (&all_pins[1]) Pin { -2, "GND", PinState::Low };
        pin_count = 2;
    }

    static Circuit _the;
};

template<typename D>
    requires std::derived_from<D, Device>
void test_device()
{
    Circuit &circuit = Circuit::the();
    auto    *chip = circuit.add_component<D>();
    chip->test_setup(circuit);
    auto t = circuit.start_simulation();
    chip->test_run(circuit);
    circuit.stop();
    t.join();
}

}
