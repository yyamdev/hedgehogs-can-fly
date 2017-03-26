#include "pause_state.h"
#include "world.h"
#include <iostream>
#include "gui.h"
#include "SFGUI/Button.hpp"
#include "SFGUI/Scale.hpp"
#include "build_options.h"
#include "util.h"
#include "options.h"
#include "SFGUI/Label.hpp"
#include "SFGUI/Window.hpp"
#include "SFGUI/Box.hpp"
#include "SFGUI/Separator.hpp"

StatePause::StatePause(World *world) : State(world) {
}

void StatePause::on_event(sf::Event &event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape | event.key.code == sf::Keyboard::P) {
        State::pop_state();
    }
}

void StatePause::on_tick() {
}

void StatePause::on_draw(sf::RenderWindow &window) {
}

void StatePause::on_draw_ui(sf::RenderWindow &window) {
}

void StatePause::on_gain_focus() {
    gui.RemoveAll();
    if (!world->is_paused()) world->toggle_pause();

    // create ui
    sf::Vector2f sliderSize(200.f, 32.f);

    auto guiWinMain = sfg::Window::Create(sfg::Window::Style::BACKGROUND);
    guiWinMain->SetId("winPauseMain");
    gui.Add(guiWinMain);

    auto guiBoxMain = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.f);
    guiWinMain->Add(guiBoxMain);

    auto guiLblTitle = sfg::Label::Create("Paused");
    guiLblTitle->SetId("lblPauseTitle");
    guiBoxMain->Pack(guiLblTitle);

    auto guiBoxSpace = sfg::Box::Create();
    guiBoxSpace->SetRequisition(sf::Vector2f(1.f, 50.f));
    guiBoxMain->Pack(guiBoxSpace);

    auto guiButtonBack = sfg::Button::Create("Resume");
    guiButtonBack->SetId("btnPauseBack");
    guiButtonBack->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiButtonBack->GetRequisition().x / 2.f, 500.f));
    guiButtonBack->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
        State::pop_state();
    }));
    guiBoxMain->Pack(guiButtonBack);

    auto guiButtonSelect = sfg::Button::Create("Level Select");
    guiButtonSelect->SetId("btnPauseSelect");
    guiButtonSelect->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiButtonBack->GetRequisition().x / 2.f, 500.f));
    guiButtonSelect->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
        State::pop_state();
        State::pop_state(); // pop state back to level select 
    }));
    guiBoxMain->Pack(guiButtonSelect);

    // position window at centre of screen
    // needs to be done at the end so SFGUI knows how big it has to be
    guiWinMain->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2 - guiWinMain->GetRequisition().x / 2.f, WINDOW_HEIGHT / 2 - guiWinMain->GetRequisition().y / 2.f - 75.f));

}

void StatePause::on_lose_focus() {
    if (world->is_paused()) world->toggle_pause();
}

void StatePause::on_notify(Event event, void *data) {
}