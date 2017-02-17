#include "select_state.h"
#include "world.h"
#include <iostream>

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
    // remove all entities in world
    world->remove_entity(ENTITY_TAG_ALL);

    std::cout << "level select\n";
}

void StateSelect::on_lose_focus() {
}

void StateSelect::on_notify(Event event, void *data) {
}
