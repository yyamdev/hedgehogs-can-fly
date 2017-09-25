#include "menu_state.h"
#include "world.h"
#include <iostream>
#include "select_state.h"
#include "shared_res.h"
#include "build_options.h"
#include "cursor.h"
#include "SFGUI/Button.hpp"
#include "SFGUI/Scale.hpp"
#include "SFGUI/Label.hpp"
#include "gui.h"
#include "world.h"
#include "build_options.h"
#include "options_state.h"
#include <math.h>

StateMenu::StateMenu(World *world) : State(world) {
    set_cursor(CURSOR_POINTER);
    txtTitle.loadFromFile("data/header.png");
    sprTitle.setTexture(txtTitle);
    sprTitle.setOrigin(txtTitle.getSize().x / 2.f, txtTitle.getSize().y / 2.f);
    sprTitle.setPosition(WINDOW_WIDTH / 2.f, 100.f);
}

void StateMenu::on_event(sf::Event &event) {
}

sf::Color StateMenu::get_clear_colour()
{
    return sf::Color(5, 5, 5);
}

void StateMenu::on_tick() {
    static float time = 0.f;
    static float size = 1.f;
    static float rot  = 0.f;
    
    size += sin(time) * .002f;
    rot   = sin(time) * 2.f;
    
    sprTitle.setScale(sf::Vector2f(size, size));
    sprTitle.setRotation(rot);

    time += .05f;
}

void StateMenu::on_draw(sf::RenderWindow &window) {
    window.draw(sprTitle);
}

void StateMenu::on_draw_ui(sf::RenderWindow &window) {
}

void StateMenu::on_gain_focus() {
    // remove all entities in world & clear ui
    world->remove_entity(ENTITY_TAG_ALL);
    gui.RemoveAll();

    // create ui
    auto guiButtonPlay = sfg::Button::Create("Play");
    guiButtonPlay->SetId("btnMenuPlay");
    guiButtonPlay->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiButtonPlay->GetRequisition().x / 2.f, 275.f));
    gui.Add(guiButtonPlay);
    guiButtonPlay->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
        State::push_state(new StateSelect(world));
    }));

    auto guiButtonOptions = sfg::Button::Create("Options");
    guiButtonOptions->SetId("btnMenuOptions");
    guiButtonOptions->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiButtonOptions->GetRequisition().x / 2.f, 375.f));
    gui.Add(guiButtonOptions);
    guiButtonOptions->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
        State::push_state(new StateOptions(world));
    }));
}

void StateMenu::on_lose_focus() {
}

void StateMenu::on_notify(Event event, void *data) {
}
