#include "options_state.h"
#include "world.h"

#include <iostream>
#include "gui.h"
#include "SFGUI/Button.hpp"
#include "build_options.h"
#include "util.h"

StateOptions::StateOptions(World *world) : State(world) {
}

void StateOptions::on_event(sf::Event &event) {
}

void StateOptions::on_tick() {
}

void StateOptions::on_draw(sf::RenderWindow &window) {
}

void StateOptions::on_draw_ui(sf::RenderWindow &window) {
}

void StateOptions::on_gain_focus() {
    // remove all entities in world & clear ui
    world->remove_entity(ENTITY_TAG_ALL);
    gui.RemoveAll();

    // create ui
    auto guiButtonBack = sfg::Button::Create("Back");
    guiButtonBack->SetId("btnOptionsBack");
    guiButtonBack->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiButtonBack->GetRequisition().x / 2.f, 500.f));
    gui.Add(guiButtonBack);
    guiButtonBack->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([] (void) {
        State::pop_state();
    }));
}

void StateOptions::on_lose_focus() {
}

void StateOptions::on_notify(Event event, void *data) {
}
