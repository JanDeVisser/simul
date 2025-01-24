/*
 * Copyright (c) 2025, Jan de Visser <jan@finiandarcy.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "System.h"
#include "Addr_Register.h"
#include "ControlBus.h"
#include "GP_Register.h"
#include "Graphics.h"
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
    cards.emplace_back(std::move(make_Mem_Register(*this)));
    cards.emplace_back(std::move(make_Monitor(*this)));

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
        DrawRectangleRoundedLines(outline, 0.2, 10, 2, DARKGREEN);
        card.board->render();
    }
}

}
