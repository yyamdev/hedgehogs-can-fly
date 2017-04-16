#include "select_state.h"
#include "world.h"
#include <iostream>
#include "gui.h"
#include "SFGUI/Button.hpp"
#include "SFGUI/Label.hpp"
#include "build_options.h"
#include "util.h"
#include "play_state.h"

StateSelect::StateSelect(World *world) : State(world) {
}

void StateSelect::on_event(sf::Event &event) {
}

void StateSelect::on_tick() {
}

void StateSelect::on_draw(sf::RenderWindow &window) {
}

void StateSelect::on_draw_ui(sf::RenderWindow &window) {
}

void StateSelect::on_gain_focus() {
    // remove all entities in world & clear ui
    world->remove_entity(ENTITY_TAG_ALL);
    gui.RemoveAll();

    // create ui
    auto guiLblTitle = sfg::Label::Create("Select Level");
    guiLblTitle->SetId("lblSelectTitle");
    guiLblTitle->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiLblTitle->GetRequisition().x / 2.f, 35.f));
    gui.Add(guiLblTitle);

    float w = 64.f, h = 64.f; // dimensions of buttons
    float g = 32.f; // xy gap between buttons
    int rows = 3, cols = 5;
    float sub = cols / 2.f - 0.5f;
    float yOffset = 200.f;
    float xOffset = WINDOW_WIDTH / 2 - sub * (w + g);
    sf::Vector2f position = sf::Vector2f(xOffset, yOffset);
    sf::Vector2f right = sf::Vector2f(w + g, 0.f);
    sf::Vector2f down = sf::Vector2f(0.f, h + g);
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            auto guiButtonLvl = sfg::Button::Create(util::to_string((y * cols + x) + 1));
            guiButtonLvl->SetId("btnSelectLevel");
            guiButtonLvl->SetRequisition(sf::Vector2f(w, h));
            guiButtonLvl->SetPosition(position - sf::Vector2f(w / 2, h / 2));
            gui.Add(guiButtonLvl);
            guiButtonLvl->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this, x, y, cols] (void) {
                State::push_state(new StatePlay(world, (y * cols + x) + 1));
            }));
            position += right;
        }
        position += down;
        position.x = xOffset;
    }

    auto guiButtonBack = sfg::Button::Create("Back");
    guiButtonBack->SetId("btnSelectBack");
    guiButtonBack->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiButtonBack->GetRequisition().x / 2.f, 500.f));
    gui.Add(guiButtonBack);
    guiButtonBack->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([] (void) {
        State::pop_state();
    }));
}

void StateSelect::on_lose_focus() {
}

void StateSelect::on_notify(Event event, void *data) {
}
