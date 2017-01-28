#include "play_state.h"
#include "world.h"
#include "terrain_entity.h"
#include "util.h"
#include <iostream>
#include "ball_entity.h"
#include "debug_draw.h"
#include "build_options.h"

StatePlay::StatePlay(World *world, std::string filename) : State(world) {
    // remove all entities in world
    world->remove_entity(ENTITY_TAG_ALL);
    
    // create terrain
    terrain = new EntityTerrain(2.f, filename);
    world->add_entity(terrain);
    
    // add ball & centre camera
    EntityBall *player = new EntityBall(terrain->playerSpawn, sf::Vector2f());
    world->add_entity(player);
    sf::Vector2f screenSize((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
    world->camera = (player->position - screenSize / 2.f);

    // reset wind change clock
    windClock.restart();
}

void StatePlay::on_event(sf::Event &event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::B) {
        world->toggle_pause();
    }
}

void StatePlay::on_tick() {
    if (windClock.getElapsedTime().asSeconds() > 5.f) {
        world->wind = sf::Vector2f(util::rnd(-0.03f, 0.03f), 0.f);
        notify(EVENT_WIND_CHANGE, (void*)&world->wind);
        windClock.restart();
    }
}

void StatePlay::on_draw(sf::RenderWindow &window) {
    hud.draw(window);
    if (DRAW_TERRAIN_NORMALS) {
        sf::Vector2i mouseI = sf::Mouse::getPosition(window);
        sf::Vector2f mouse;
        mouse.x = (float)mouseI.x;
        mouse.y = (float)mouseI.y;
        sf::Vector2f normal = terrain->get_normal(mouse + world->camera);
        draw_vector(mouse, normal, 50.f, sf::Color::Yellow, window);
    }
}

void StatePlay::on_gain_focus() {
}

void StatePlay::on_lose_focus() {
}

void StatePlay::on_notify(Event event, void *data) {
}