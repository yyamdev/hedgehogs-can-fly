#include "options_state.h"
#include "world.h"
#include <iostream>
#include "gui.h"
#include "SFGUI/Button.hpp"
#include "SFGUI/Scale.hpp"
#include "build_options.h"
#include "util.h"
#include "options.h"

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

    options::load();

    // create ui
    auto guiSliderMusic = sfg::Scale::Create(0.f, 1.f, 0.01f);
    guiSliderMusic->SetId("sclOptionsMusic");
    guiSliderMusic->SetRequisition(sf::Vector2f(200.f, 32.f));
    guiSliderMusic->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2 - guiSliderMusic->GetRequisition().x / 2.f, 200.f));
    guiSliderMusic->SetValue(options::musicVolume);
    gui.Add(guiSliderMusic);
    guiSliderMusic->GetSignal(sfg::Scale::OnMouseLeftRelease).Connect(std::bind([guiSliderMusic] (void) {
        options::musicVolume = guiSliderMusic->GetValue();
    }));

    auto guiButtonBack = sfg::Button::Create("Back");
    guiButtonBack->SetId("btnOptionsBack");
    guiButtonBack->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiButtonBack->GetRequisition().x / 2.f, 500.f));
    gui.Add(guiButtonBack);
    guiButtonBack->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([guiSliderMusic] (void) {
        guiSliderMusic->Show(false); // neccessary to stop this widget from displaying in other menus, possibly an SFGUI bug?
        State::pop_state();
    }));
}

void StateOptions::on_lose_focus() {
}

void StateOptions::on_notify(Event event, void *data) {
}
