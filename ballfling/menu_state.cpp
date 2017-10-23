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
#include "particle.h"
#include "util.h"

StateMenu::StateMenu(World *world) : State(world) {
    set_cursor_visible(true);
    set_cursor(CURSOR_POINTER);
    txtTitle.loadFromFile("data/header.png");
    sprTitle.setTexture(txtTitle);
    sprTitle.setOrigin(txtTitle.getSize().x / 2.f, txtTitle.getSize().y / 2.f);
    sprTitle.setPosition(WINDOW_WIDTH / 2.f, 150.f);
}

void StateMenu::on_event(sf::Event &event) {
}

void StateMenu::on_tick() {
    static float time = 0.f;
    static float size = 1.f;
    static float rot  = 0.f;
    
    size += sin(time) * .002f;
    rot   = sin(time) * 1.6f;
    
    sprTitle.setScale(sf::Vector2f(size, size));
    sprTitle.setRotation(rot);

    time += .05f;


    static sf::Vector2f pos(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);
    float speed = 4.f;
    static sf::Vector2f vel(util::rnd(-speed, speed), util::rnd(-speed, speed));
    sf::Vector2f next = pos + vel;
    if (next.x < 0.f || next.x > WINDOW_WIDTH) vel.x = -vel.x;
    if (next.y < 0.f || next.y > WINDOW_HEIGHT) vel.y = -vel.y;
    pos += vel;
    add_particle(pos, sf::Vector2f(), sf::Vector2f(0.f, 0.f), sf::Color(95, 205, 228), 240);

    particles_tick(sf::Vector2f());
}

void StateMenu::on_draw(sf::RenderWindow &window) {
    particles_draw(window, sf::Vector2f());
    window.draw(sprTitle);
}

void StateMenu::on_draw_ui(sf::RenderWindow &window) {
}

void StateMenu::on_gain_focus() {
    // remove all entities in world & clear ui
    world->remove_entity(ENTITY_TAG_ALL);
    gui.RemoveAll();

    // Signal to play music
    notify(EVENT_ENTER_MENU, NULL);

    // create ui
    auto guiButtonPlay = sfg::Button::Create("Play");
    guiButtonPlay->SetId("btnMenuPlay");
    guiButtonPlay->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiButtonPlay->GetRequisition().x / 2.f, 325.f));
    gui.Add(guiButtonPlay);
    guiButtonPlay->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
        State::push_state(new StateSelect(world));
    }));

    auto guiButtonOptions = sfg::Button::Create("Options");
    guiButtonOptions->SetId("btnMenuOptions");
    guiButtonOptions->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiButtonOptions->GetRequisition().x / 2.f, 425.f));
    gui.Add(guiButtonOptions);
    guiButtonOptions->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
                State::push_state(new StateOptions(world, true, sf::Color::Black));
    }));
}

void StateMenu::on_lose_focus() {
}

void StateMenu::on_notify(Event event, void *data) {
}
