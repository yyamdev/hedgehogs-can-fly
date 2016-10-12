#include "play_state.h"
#include "world.h"
#include "dude_entity.h"
#include "terrain_entity.h"
#include "keyboard_driver.h"
#include "ai_driver.h"

StatePlay::StatePlay(World *world) : State(world) {
    // set up game
    EntityTerrain* terrain = new EntityTerrain(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT), 2.f);
    //terrain->generate_flat();
    terrain->generate_normal(500.f, 100.f);
    world->add_entity(terrain);

    EntityDude* dudePlayer = new EntityDude(sf::Vector2f(32.f, 0), PLAYER_NUMBER);
    dudePlayer->set_driver(std::make_unique<KeyboardDriver>(dudePlayer));
    world->add_entity(dudePlayer);

    EntityDude* dudeAi = new EntityDude(sf::Vector2f(WINDOW_WIDTH - 32.f, 0), AI_NUMBER);
    dudeAi->set_driver(std::make_unique<AiDriver>(dudeAi, terrain));
    world->add_entity(dudeAi);
}

void StatePlay::on_event(sf::Event &event) {
}

void StatePlay::on_tick() {
}

void StatePlay::on_draw(sf::RenderWindow &window) {
    hud.draw(window);
}

void StatePlay::on_gain_focus() {
}

void StatePlay::on_lose_focus() {
}