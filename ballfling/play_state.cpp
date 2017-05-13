#include "play_state.h"
#include "world.h"
#include "terrain_entity.h"
#include "util.h"
#include <iostream>
#include "ball_entity.h"
#include "debug_draw.h"
#include "build_options.h"
#include "gui.h"
#include "options_state.h"
#include "win_state.h"
#include "pause_state.h"

std::string level_num_to_filename(int levelNum) {
    return std::string("data/lvl" + util::to_string(levelNum) + ".png");
}

StatePlay::StatePlay(World *world, int levelNum) : State(world) {
    this->levelNum = levelNum;
    filename = level_num_to_filename(levelNum);

    //levelColour = sf::Color::Red;
    levelColour = sf::Color(99, 155, 255);

    gui.RemoveAll();
    world->remove_entity(ENTITY_TAG_ALL);

    // create terrain
    terrain = new EntityTerrain(2.f, filename, levelColour);
    if (terrain->error) {
        return;
    }
    world->add_entity(terrain);

    // add ball & centre camera
    player = new EntityBall(terrain->playerSpawn, sf::Vector2f(), levelColour);
    world->add_entity(player);
    sf::Vector2f screenSize((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
    world->camera = (player->position - screenSize / 2.f);

    completed = false;
}

void StatePlay::on_event(sf::Event &event) {
    if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::P)) {
        State::push_state(new StatePause(world));
    }
}

void StatePlay::on_tick() {
    if (terrain->error) State::pop_state(); 
    if (completed) {
        if (!world->is_paused()) world->toggle_pause(); // pause world
        State::push_state(new StateWin(world, levelNum));
    }
}

void StatePlay::on_draw(sf::RenderWindow &window) {
    if (DRAW_TERRAIN_NORMALS) {
        sf::Vector2i mouseI = sf::Mouse::getPosition(window);
        sf::Vector2f mouse;
        mouse.x = (float)mouseI.x;
        mouse.y = (float)mouseI.y;
        sf::Vector2f normal = terrain->get_normal(mouse + world->camera);
        draw_vector(mouse, normal, 50.f, sf::Color::Yellow, window);
    }
}

void StatePlay::on_draw_ui(sf::RenderWindow &window) {
    hud.draw(window, world->camera, levelColour);
}

void StatePlay::on_gain_focus() {
    gui.RemoveAll();
    if (world->is_paused()) world->toggle_pause();
}

void StatePlay::on_lose_focus() {
}

void StatePlay::on_notify(Event event, void *data) {
    if (event == EVENT_BALL_HIT_FINISH) {
        completed = true;
    }
}