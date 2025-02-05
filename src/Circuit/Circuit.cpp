/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "Circuit.h"
#include <algorithm>

namespace Simul {

void Circuit::initialize(std::string const &name)
{
    assert(status == SimStatus::Unstarted || status == SimStatus::Done);
    Device::name = name;
    for (auto *c : components) {
        delete c;
    }
    components.clear();
    pin_count = 2;
}

void Circuit::start()
{
    if (status == SimStatus::Unstarted) {
        status = SimStatus::Started;
    }
}

void Circuit::stop()
{
    if (status == SimStatus::Started) {
        status = SimStatus::Stopping;
    }
}

void Circuit::done()
{
    if (status == SimStatus::Stopping) {
        status = SimStatus::Done;
    }
}

Pin *Circuit::allocate_pin(int nr, std::string const &pin_name, PinState state)
{
    assert(pin_count < all_pins.size());
    auto *ret = &all_pins[pin_count++];
    return new (ret) Pin { nr, pin_name, state };
}

void recurse_components(Device *dev, auto callback)
{
    for (auto *c : dev->components) {
        recurse_components(c, callback);
    }
    callback(dev);
};

size_t Circuit::simulate(duration d)
{
    size_t ret = 0;
    for (auto ix = pin_count - 1; ix < pin_count; --ix) {
        if (all_pins[ix].update(d)) {
            ++ret;
        }
    }
    for (auto ix = 0; ix < pin_count; ++ix) {
        auto &p = all_pins[ix];
        if (all_pins[ix].state != all_pins[ix].new_state && p.on_change) {
            (*p.on_change)(&p, d);
        }
    }
    std::function<void(Device *)> recurse = [&recurse, d](Device *dev) -> void {
        for (auto *c : dev->components) {
            recurse(c);
        }
        if (dev->simulate_device) {
            (*dev->simulate_device)(dev, d);
        }
    };
    recurse_components(this, [d](Device *dev) {
        if (dev->simulate_device.has_value()) {
            (dev->simulate_device.value())(dev, d);
        }
    });
    for (auto ix = 0; ix < pin_count; ++ix) {
        auto &p = all_pins[ix];
        if (p.on_drive) {
            (*p.on_drive)(&p, d);
        }
        if (p.new_driving && p.drive && p.new_state != PinState::Z) {
            p.drive->new_state = p.new_state;
        }
    }
    for (auto ix = 0; ix < pin_count; ++ix) {
        all_pins[ix].state = all_pins[ix].new_state;
        all_pins[ix].driving = all_pins[ix].new_driving;
    }
    return ret;
}

std::thread Circuit::start_simulation()
{
    std::unique_lock lock(yield_mutex);

    for (auto ix = 0; ix < pin_count; ++ix) {
        Pin &p = all_pins[ix];
        if (p.on_update) {
            (*p.on_update)(&p, 0ms);
        }
        if (p.on_change) {
            (*p.on_change)(&p, 0ms);
        }
    }
    recurse_components(this, [](Device *dev) {
        if (dev->simulate_device) {
            (*dev->simulate_device)(dev, 0ms);
        }
    });
    for (auto ix = 0; ix < pin_count; ++ix) {
        all_pins[ix].new_state = all_pins[ix].state;
        all_pins[ix].new_driving = all_pins[ix].driving;
    }

    std::thread t { [&]() {
        if (status != SimStatus::Unstarted && status != SimStatus::Done) {
            return;
        }
        auto    start = std::chrono::high_resolution_clock::now();
        status = SimStatus::Starting;
        do {
            {
                std::unique_lock lock(yield_mutex);
                auto             now { std::chrono::high_resolution_clock::now() };
                simulate(now - start);
                if (status == SimStatus::Starting) {
                    status = SimStatus::Started;
                }
            }
            yielder.notify_all();
            std::this_thread::sleep_for(1ns);
        } while (status != SimStatus::Stopping);
        status = SimStatus::Done;
    } };
    yielder.wait(lock);
    return t;
}

void Circuit::yield()
{
    std::unique_lock lock(yield_mutex);
    yielder.wait(lock);
}

Circuit Circuit::_the {};

Circuit &Circuit::the()
{
    return _the;
}
}
