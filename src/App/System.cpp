/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "System.h"
#include "ALU.h"
#include "Addr_Register.h"
#include "Circuit/Graphics.h"
#include "ControlBus.h"
#include "GP_Register.h"
#include "Mem_Register.h"
#include "Monitor.h"

namespace Simul {

System::System(Font font)
    : circuit(Circuit::the())
    , font(font)
{
    bus = make_backplane(*this);
    cards.emplace_back(std::move(make_GP_Register(*this, 0)));
    cards.emplace_back(std::move(make_GP_Register(*this, 1)));
    cards.emplace_back(std::move(make_GP_Register(*this, 2)));
    cards.emplace_back(std::move(make_GP_Register(*this, 3)));
    cards.emplace_back(std::move(make_Addr_Register(*this, 8)));
    cards.emplace_back(std::move(make_Addr_Register(*this, 9)));
    cards.emplace_back(std::move(make_Addr_Register(*this, 10)));
    cards.emplace_back(std::move(make_Addr_Register(*this, 11)));
    cards.emplace_back(std::move(make_Addr_Register(*this, 12)));
    auto &mem_card = cards.emplace_back(std::move(make_Mem_Register(*this)));
    rom = dynamic_cast<Mem_Register *>(mem_card.circuit)->U10;
    ram = dynamic_cast<Mem_Register *>(mem_card.circuit)->U9;
    cards.emplace_back(std::move(make_ALU(*this)));
    auto &mon_card = cards.emplace_back(std::move(make_Monitor(*this)));
    monitor = dynamic_cast<Monitor *>(mon_card.circuit);

    for (auto ix = 0; ix < cards.size(); ++ix) {
        auto &card = cards[ix];
        card.board->add_text(2, 70, card.circuit->name, 0.0,
            [this](Text *) -> void {
                this->current_card.reset();
            });
        card.edge->add_text(2, 82, card.circuit->name, 0.0,
            [this, ix](Text *) -> void {
                this->current_card = ix;
            });
    }

    layout();

    bus->CLK->state = PinState::Low;
    bus->XDATA_->state = PinState::High;
    bus->XADDR_->state = PinState::High;
    bus->IO_->state = PinState::High;
    bus->set_op(0x00);
    bus->set_put(0x00);
    bus->set_get(0x01);
    bus->set_addr(0);
}

std::unique_ptr<Board> System::make_board()
{
    auto board = std::make_unique<Board>(Circuit::the(), font);
    board->font = font;
    return board;
}

void System::layout()
{
    auto height = backplane->size.y;
    auto edge_width { 0.0f };

    for (auto &card : cards) {
        height = std::max(height, card.edge->size.y);
        edge_width += card.edge->size.x;
    }

    size.y = height + 2 * PITCH;
    size.x = backplane->size.x + edge_width + (4.0f + static_cast<float>(cards.size()) - 1.0f) * PITCH;

    auto offset_x { PITCH };
    backplane->layout(offset_x, PITCH, backplane->size.x, height);
    offset_x += backplane->size.x + PITCH;
    for (auto &card : cards) {
        card.board->layout(
            (size.x - card.board->size.x) / 2,
            (size.y - card.board->size.y) / 2,
            card.board->size.x,
            card.board->size.y);
        card.edge->layout(offset_x, PITCH, card.edge->size.x, height);
        offset_x += card.edge->size.x + PITCH;
    }
}

void System::handle_input()
{
    if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
        current_step = 0;
        bus->enable_oscillator();
    }
    if (CheckCollisionPointRec(GetMousePosition(), backplane->rect)) {
        backplane->handle_input();
        return;
    }
    if (current_card) {
        auto &card = cards[*current_card];
        if (CheckCollisionPointRec(GetMousePosition(), card.board->rect)) {
            card.board->handle_input();
            return;
        }
    }
    int ix = 0;
    for (auto &card : cards) {
        if (CheckCollisionPointRec(GetMousePosition(), card.edge->rect)) {
            card.edge->handle_input();
            return;
        }
        ix += 1;
    }
}

void System::render()
{
    backplane->render();
    //    cards[current_card].board->render();
    auto ix = 0;
    for (auto &card : cards) {
        card.edge->render();
        ++ix;
    }
    if (current_card) {
        auto &card = cards[*current_card];
        auto &rect = card.board->rect;
        auto  outline = Rectangle { rect.x - PITCH * 0.5f, rect.y - PITCH * 0.5f, rect.width + PITCH, rect.height + PITCH };
        DrawRectangleRounded(outline, 0.2, 10, DARKGREEN);
        outline = Rectangle { rect.x - PITCH * 0.25f, rect.y - PITCH * 0.25f, rect.width + PITCH * 0.5f, rect.height + PITCH * 0.5f };
        DrawRectangleRoundedLines(outline, 0.2, 10, DARKGREEN);
        card.board->render();
    }
}

std::thread System::simulate()
{
    if (!microcode.empty()) {
        bus->enable_oscillator();
        bus->oscillator->on_low = [this](Oscillator *) {
            if (microcode.empty() || current_step >= microcode.size()) {
                bus->disable_oscillator();
                return;
            }
            auto &step = microcode[current_step];
            switch (step.action) {
            case MicroCodeAction::XData:
                bus->XDATA_->new_state = PinState::Low;
                bus->XADDR_->new_state = PinState::High;
                bus->IO_->new_state = PinState::High;
                break;
            case MicroCodeAction::XAddr:
                bus->XDATA_->new_state = PinState::High;
                bus->XADDR_->new_state = PinState::Low;
                bus->IO_->new_state = PinState::High;
                break;
            default:
                break;
            }
            switch (step.payload.index()) {
            case 0: {
                auto &tx = std::get<Transfer>(step.payload);
                bus->set_get(tx.get_from);
                bus->set_put(tx.put_to);
                bus->set_op(tx.op_bits);
            } break;
            case 2: {
                auto &mon = std::get<MonitorValue>(step.payload);
                set_pins(monitor->SW1, mon.d);
                set_pins(monitor->SW2, mon.a);
            } break;
            default:
                break;
            }
            current_step++;
            if (current_step >= microcode.size()) {
                bus->disable_oscillator();
            }
        };

        for (auto &step : microcode) {
            if (step.action == MicroCodeAction::SetMem) {
                auto &block = std::get<MemBlock>(step.payload);
                auto  addr = block.address;
                for (auto bit : block.bytes) {
                    if (addr & 0x8000) {
                        rom->bytes[addr & 0x7FFF] = bit;
                    } else {
                        ram->bytes[addr] = bit;
                    }
                    ++addr;
                }
            }
        }
    }
    auto t = circuit.start_simulation();
    return t;
}

}
