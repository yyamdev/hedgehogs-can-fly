#include "play_state.h"
#include "world.h"
#include "terrain_entity.h"
#include "util.h"
#include "ball_entity.h"
#include "gate.h"
#include "build_options.h"
#include "gui.h"
#include "options_state.h"
#include "win_state.h"
#include "pause_state.h"
#include <string>
#include "imgui.h"
#include "debug_draw.h"
#include "particle.h"
#include "shared_res.h"
#include <fstream>
#include "timer_entity.h"
#include "enemy_entity.h"

std::string level_num_to_filename(int levelNum) {
    return std::string("data/lvl" + util::to_string(levelNum) + ".png");
}

StatePlay::StatePlay(World *world, int levelNum) : State(world) {
    this->levelNum = levelNum;
    filename = level_num_to_filename(levelNum);

    restartOnResume = false;

    if (levelNum <= 4) {
        levelColour = sf::Color(99, 155, 255);
        backgroundColor = sf::Color(34, 32, 52);
    }
    else if (levelNum <= 9) {
        levelColour = sf::Color(203, 219, 252);
        backgroundColor = sf::Color(50, 60, 57);
    }
    else {
        levelColour = sf::Color(55, 148, 110);
        backgroundColor = sf::Color(69, 40, 60);
    }

    gui.RemoveAll();
    world->remove_entity(ENTITY_TAG_ALL);

    // create terrain
    terrain = new EntityTerrain(2.f, filename, levelColour);
    if (terrain->error) {
        return;
    }
    world->add_entity(terrain);

    world->add_entity(new EntityTimer());

    // add ball & centre camera
    player = new EntityBall(terrain->playerSpawn, sf::Vector2f(), levelColour);
    world->add_entity(player);
    sf::Vector2f screenSize((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
    world->camera = (player->position - screenSize / 2.f);

    // add enemy
    world->add_entity(new EntityEnemy(player->position, &player->position));

    // add gates
    gates = add_gates_from_file("data/lvl" + util::to_string(levelNum) + "gates.txt");
    //gate = new EntityGate(sf::Vector2f(650.f, 750.f), 25.f, 64.f, 2.f, levelColour);

    completed = false;
}

sf::Color StatePlay::get_clear_colour() {
    return backgroundColor;
}

void StatePlay::on_event(sf::Event &event) {
    if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::P)) {
        notify(EVENT_PLAY_PAUSE, NULL);
        State::push_state(new StatePause(world, &restartOnResume));
    }
}

void StatePlay::on_tick() {
    if (!world->is_paused()) particles_tick(world->camera);
    if (terrain->error) State::pop_state(); 
    if (completed) {
        if (!world->is_paused()) world->toggle_pause(); // pause world
        notify(EVENT_PLAY_PAUSE, NULL);
        State::push_state(new StateWin(world, levelNum, &restartOnResume));
    }
    playerPosition = player->position;
    playerVelocity = player->velocity;
}

void StatePlay::on_draw(sf::RenderWindow &window) {
    if (edit && ImGui::CollapsingHeader("Gates")) {
        if (ImGui::Button("Write")) {
            std::ofstream file("data/lvl" + util::to_string(levelNum) + "gates.txt");
            file << serialise_gates_to_string(gates);
            file.close();
        }
        if (ImGui::Button("Add")) {
            EntityGate *gate = new EntityGate(player->position - sf::Vector2f(0.f, 64.f), 0.f, 64.f, 2.f, levelColour);
            gates.push_back(gate);
            world->add_entity(gate);
        }
        ImGui::Text("A to select all");
        ImGui::Text("D to deselect all");
        int count = 0;
        for (auto &gate : gates) {
            if (gate->selected) {
                ImGui::DragFloat("x", &gate->position.x);
                ImGui::DragFloat("y", &gate->position.y);
                ImGui::DragFloat("angle", &gate->angle);
                ImGui::DragFloat("size", &gate->size);
                ImGui::DragFloat("strength", &gate->strength, .002f, 0.f, 16.f);
                if (ImGui::Button("Remove")) {
                    gate->remove = true;
                }
                ImGui::Separator();
            }
            ++count;
        }
        for (auto it = gates.begin(); it != gates.end(); ) {
            if ((*it)->remove) {
                gates.erase(it);
                it = gates.begin();
            }
            else ++it;
        }
    }
    particles_draw(window, world->camera);
}

void StatePlay::on_draw_ui(sf::RenderWindow &window) {
    hud.draw(window, world->camera, levelColour);
}

void StatePlay::on_gain_focus() {
    gui.RemoveAll();
    if (restartOnResume) {
        State::change_state(new StatePlay(world, levelNum));
        return;
    }
    if (world->is_paused()) world->toggle_pause();
    notify(EVENT_PLAY_RESUME, NULL);
}

void StatePlay::on_lose_focus() {
}

void StatePlay::on_notify(Event event, void *data) {
    if (event == EVENT_BALL_HIT_FINISH) {
        completed = true;
    }
}

std::vector<EntityGate*> StatePlay::add_gates_from_file(std::string filename) {
    std::vector<EntityGate*> vec;
    std::ifstream file(filename);
    if (!file.is_open()) return vec;
    while (!file.eof()) {
        // <x> <y> <angle> <size> <str>
        sf::Vector2f position;
        float strength, size, angle;
        file >> position.x >> position.y;
        file >> angle >> size >> strength;
        EntityGate *gate = new EntityGate(position, angle, size, strength, levelColour);
        vec.push_back(gate);
        world->add_entity(gate);
    }
    file.close();
    return vec;
}

std::string StatePlay::serialise_gates_to_string(std::vector<EntityGate*> gates) {
    std::string s;
    bool first = true;
    for (auto &gate : gates) {
        if (!first) s = s + " ";
        s = s + util::to_string(gate->position.x) + " " + util::to_string(gate->position.y) + " ";
        s = s + util::to_string(gate->angle) + " " + util::to_string(gate->size) + " " + util::to_string(gate->strength);
        first = false;
    }
    return s;
}
