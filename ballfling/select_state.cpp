#include "select_state.h"
#include "world.h"
#include <iostream>
#include "gui.h"
#include "SFGUI/Button.hpp"
#include "build_options.h"
#include "util.h"

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
    float w = 64.f, h = 64.f; // dimensions of buttons
    float g = 32.f; // xy gap between buttons
    float yOffset = 200.f;
    float xOffset = WINDOW_WIDTH / 2 - (2 * w) - (2 * g);
    sf::Vector2f position = sf::Vector2f(xOffset, yOffset);
    std::cout << util::vec2_to_str(position) << std::endl;
    sf::Vector2f right = sf::Vector2f(w + g, 0.f);
    sf::Vector2f down = sf::Vector2f(0.f, h + g);
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 5; ++x) {
            auto guiButtonLvl = sfg::Button::Create(util::to_string((y * 5 + x) + 1));
            guiButtonLvl->SetId("btnSelectLevel");
            guiButtonLvl->SetRequisition(sf::Vector2f(w, h));
            guiButtonLvl->SetPosition(position - sf::Vector2f(w / 2, h / 2));
            gui.Add(guiButtonLvl);
            guiButtonLvl->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this, x, y] (void) {
                std::cout << (y * 5 + x) + 1 << std::endl;
            }));
            position += right;
        }
        position += down;
        position.x = xOffset;
    }
}

void StateSelect::on_lose_focus() {
}

void StateSelect::on_notify(Event event, void *data) {
}
