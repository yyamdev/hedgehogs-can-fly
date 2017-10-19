#include "end_state.h"
#include "world.h"
#include <iostream>
#include "gui.h"
#include "SFGUI/Button.hpp"
#include "build_options.h"
#include "util.h"
#include "SFGUI/Label.hpp"
#include "SFGUI/Window.hpp"
#include "SFGUI/Box.hpp"
#include "SFGUI/Separator.hpp"
#include "play_state.h"

StateEnd::StateEnd(World *world) : State(world) {
}

void StateEnd::on_event(sf::Event &event) {
}

void StateEnd::on_tick() {
}

void StateEnd::on_draw(sf::RenderWindow &window) {
}

void StateEnd::on_draw_ui(sf::RenderWindow &window) {
}

void StateEnd::on_gain_focus() {
    gui.RemoveAll();

    // create ui
    auto guiWinMain = sfg::Window::Create(sfg::Window::Style::BACKGROUND);
    guiWinMain->SetId("winWinMain");
    gui.Add(guiWinMain);

    auto guiBoxMain = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.f);
    guiWinMain->Add(guiBoxMain);

    auto guiLblTitle = sfg::Label::Create("You Died!");
    guiLblTitle->SetId("lblDeathTitle");
    guiBoxMain->Pack(guiLblTitle);

    auto guiBoxSpace = sfg::Box::Create();
    guiBoxSpace->SetRequisition(sf::Vector2f(1.f, 50.f));
    guiBoxMain->Pack(guiBoxSpace);

    auto guiBoxButtons = sfg::Box::Create();
    guiBoxMain->Pack(guiBoxButtons);

    auto guiButtonSelect = sfg::Button::Create("Level Select");
    guiButtonSelect->SetId("btnDeathSelect");
    guiButtonSelect->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([] (void) {
        State::pop_state(); // pop state back to level select
    }));
    guiBoxButtons->Pack(guiButtonSelect);

    // position window at centre of screen
    // needs to be done at the end so SFGUI knows how big it has to be
    guiWinMain->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2 - guiWinMain->GetRequisition().x / 2.f, WINDOW_HEIGHT / 2 - guiWinMain->GetRequisition().y / 2.f - 75.f));
}

void StateEnd::on_lose_focus() {
    if (world->is_paused()) world->toggle_pause();
}

void StateEnd::on_notify(Event event, void *data) {
}
