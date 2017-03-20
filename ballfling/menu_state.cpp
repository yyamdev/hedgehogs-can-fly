#include "menu_state.h"
#include "world.h"
#include <iostream>
#include "select_state.h"
#include "shared_res.h"
#include "build_options.h"
#include "cursor.h"
#include "SFGUI/Button.hpp"
#include "SFGUI/Label.hpp"
#include "gui.h"
#include "world.h"
#include "build_options.h"
#include "options_state.h"

StateMenu::StateMenu(World *world) : State(world) {
    set_cursor(CURSOR_POINTER);
}

void StateMenu::on_event(sf::Event &event) {
}

void StateMenu::on_tick() {
}

void StateMenu::on_draw(sf::RenderWindow &window) {
}

void StateMenu::on_draw_ui(sf::RenderWindow &window) {
}

void StateMenu::on_gain_focus() {
    // remove all entities in world & clear ui
    world->remove_entity(ENTITY_TAG_ALL);
    gui.RemoveAll();

    // create ui
    auto guiLabelTitle = sfg::Label::Create("Title Here");
    guiLabelTitle->SetId("lblMenuTitle");
    guiLabelTitle->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiLabelTitle->GetRequisition().x / 2.f, 50.f));
    gui.Add(guiLabelTitle);

    auto guiButtonPlay = sfg::Button::Create("Play");
    guiButtonPlay->SetId("btnMenuPlay");
    guiButtonPlay->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiButtonPlay->GetRequisition().x / 2.f, 225.f));
    gui.Add(guiButtonPlay);
    guiButtonPlay->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
        State::push_state(new StateSelect(world));
    }));

    auto guiButtonOptions = sfg::Button::Create("Options");
    guiButtonOptions->SetId("btnMenuOptions");
    guiButtonOptions->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiButtonOptions->GetRequisition().x / 2.f, 325.f));
    gui.Add(guiButtonOptions);
    guiButtonOptions->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
        State::push_state(new StateOptions(world));
    }));
}

void StateMenu::on_lose_focus() {
}

void StateMenu::on_notify(Event event, void *data) {
}
