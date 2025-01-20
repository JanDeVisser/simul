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
    cards.emplace_back(std::move(make_Monitor(*this)));

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
    auto card_width { 0.0f };
    auto edge_width { 0.0f };

    for (auto &card : cards) {
        height = std::max(std::max(height, card.board->size.y), card.edge->size.y);
        card_width = std::max(card_width, card.board->size.x);
        edge_width += card.edge->size.x;
    }

    size.y = height + 2 * PITCH;
    size.x = backplane->size.x + edge_width + (4.0f + static_cast<float>(cards.size()) - 1.0f) * PITCH;

    auto x_offset { PITCH };
    backplane->layout(x_offset, PITCH, backplane->size.x, height);
    x_offset += backplane->size.x + PITCH;
    //    x_offset += card_width + PITCH;
    for (auto &card : cards) {
        card.board->layout(0, PITCH, card_width, height);
        card.edge->layout(x_offset, PITCH, card.edge->size.x, height);
        x_offset += card.edge->size.x + PITCH;
    }
}

void System::handle_input()
{
    if (CheckCollisionPointRec(GetMousePosition(), backplane->rect)) {
        backplane->handle_input();
        return;
    }
    if (CheckCollisionPointRec(GetMousePosition(), cards[current_card].board->rect)) {
        cards[current_card].board->handle_input();
        return;
    }
    int ix = 0;
    for (auto &card : cards) {
        if (CheckCollisionPointRec(GetMousePosition(), card.edge->rect)) {
            card.edge->handle_input();
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                current_card = ix;
            }
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
        if (ix == current_card) {
            auto &rect = card.edge->rect;
            auto  outline = Rectangle { rect.x - PITCH * 0.25f, rect.y - PITCH * 0.25f, rect.width + PITCH * 0.5f, rect.height + PITCH * 0.5f };
            DrawRectangleRounded(outline, 0.2, 10, PINK);
        }
        card.edge->render();
        ++ix;
    }
}

}
