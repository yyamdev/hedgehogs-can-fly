#include "menu_state.h"
#include "world.h"
#include <iostream>
#include "select_state.h"

StateMenu::StateMenu(World *world) : State(world) {
}

void StateMenu::on_event(sf::Event &event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return) {
        State::push_state(new StateSelect(world));
    }
}

void StateMenu::on_tick() {
}

void StateMenu::on_draw(sf::RenderWindow &window) {
}

void StateMenu::on_draw_ui(sf::RenderWindow &window) {
}

void StateMenu::on_gain_focus() {
    // remove all entities in world
    world->remove_entity(ENTITY_TAG_ALL);

    std::cout << "press enter to go to level select\n";
}

void StateMenu::on_lose_focus() {
}

void StateMenu::on_notify(Event event, void *data) {
}
