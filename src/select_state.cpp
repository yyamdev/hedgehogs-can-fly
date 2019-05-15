#include "select_state.h"
#include "world.h"
#include <iostream>
#include "gui.h"
#include "SFGUI/Button.hpp"
#include "SFGUI/Label.hpp"
#include "build_options.h"
#include "util.h"
#include "play_state.h"
#include "save.h"
#include "SFGUI/Image.hpp"
#include "cursor.h"

StateSelect::StateSelect(World *world) : State(world, "select") {
    imgLock.loadFromFile("data/lock.png");
    name = "select";
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

    set_cursor(CURSOR_POINTER);

    // Play music
    notify(EVENT_ENTER_MENU, NULL);

    // create ui
    auto guiLblTitle = sfg::Label::Create("Select Level");
    guiLblTitle->SetId("lblSelectTitle");
    guiLblTitle->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiLblTitle->GetRequisition().x / 2.f, 35.f));
    gui.Add(guiLblTitle);

    float w = 64.f, h = 64.f; // dimensions of buttons
    float g = 32.f; // xy gap between buttons
    int rows = 3, cols = 4;
    float sub = cols / 2.f - 0.5f;
    float yOffset = 200.f;
    float xOffset = WINDOW_WIDTH / 2 - sub * (w + g);
    sf::Vector2f position = sf::Vector2f(xOffset, yOffset);
    sf::Vector2f right = sf::Vector2f(w + g, 0.f);
    sf::Vector2f down = sf::Vector2f(0.f, h + g);
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            int level = (y * cols + x) + 1;
            std::string text = savegame_is_level_unlocked(level) ? util::to_string(level) : "";
            auto guiButtonLvl = sfg::Button::Create(text);
            guiButtonLvl->SetId("btnSelectLevel" + util::to_string(level));
            guiButtonLvl->SetRequisition(sf::Vector2f(w, h));
            guiButtonLvl->SetPosition(position - sf::Vector2f(w / 2, h / 2));
            if (!savegame_is_level_unlocked(level))
                guiButtonLvl->SetImage(sfg::Image::Create(imgLock));
            gui.Add(guiButtonLvl);
            guiButtonLvl->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this, level] (void) {
                        if (savegame_is_level_unlocked(level)) {
                            notify(EVENT_MENU_CLICK, NULL);
                            State::push_state(new StatePlay(world, level));
                        } else {
                            notify(EVENT_MENU_LOCKED, NULL);
                        }
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
                notify(EVENT_MENU_CLICK, NULL);
                State::pop_state();
    }));
}

void StateSelect::on_lose_focus() {
}

void StateSelect::on_notify(Event event, void *data) {
}
