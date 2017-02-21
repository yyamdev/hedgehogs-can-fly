#include "world.h"
#include <assert.h>
#include <iostream>
#include "subject.h"
#include "build_options.h"
#include "debug_draw.h"
#include "imgui.h"
#include "util.h"

World::World(sf::RenderWindow &window) {
    this->window = &window;
    paused = false;
    gravity = sf::Vector2f(0.f, GRAVITY);
    wind = sf::Vector2f (0.f, 0.f);
    clamp = false;
    notify(EVENT_WIND_CHANGE, (void*)&wind);
}

World::~World() {
    if (entities.size() > 0) {
        auto it = entities.begin();
        while (it != entities.end()) {
            Entity *ptr = (*it);
            it = entities.erase(it);
            delete ptr;
        }
    }
    assert(entities.size() == 0);
}

void World::event(sf::Event &e) {
    for (auto &entity : entities) {
        entity->event(e);
    }
}

void World::tick() {
    if (paused) return;

    size_t vectorLength = entities.size(); // size might change as we go through
    if (vectorLength > 0) {
        auto it = entities.begin();
        while (it != entities.end()) {
            (*it)->tick(entities);
            float rad = 200.f;
            /*
            if ((*it)->position.x < -rad || (*it)->position.x > (float)WINDOW_WIDTH  + rad ||
                (*it)->position.y < -rad || (*it)->position.y > (float)WINDOW_HEIGHT + rad)
            {
                // remove entity if too far off screen
                (*it)->remove = true;
            }
            */
            if ((*it)->remove) {
                Entity *ptr = (*it);
                it = entities.erase(it);
                delete ptr;
            } else ++it;
        }
    }

    while (entityAddQueue.size() > 0) {
        entities.push_back(entityAddQueue.front());
        entityAddQueue.pop();
    }

    if (clamp) {
        if (camera.x < clampPos.x) camera.x = clampPos.x;
        if (camera.y < clampPos.y) camera.y = clampPos.y;
        if (camera.x + WINDOW_WIDTH  > clampPos.x + clampSize.x) camera.x = clampPos.x + clampSize.x - WINDOW_WIDTH;
        if (camera.y + WINDOW_HEIGHT > clampPos.y + clampSize.y) camera.y = clampPos.y + clampSize.y - WINDOW_HEIGHT;
    }
}

void World::draw() {
    for (auto &entity : entities) {
        entity->draw(*window);
    }

    if (edit) {
        ImGui::Begin("World");
        ImGui::InputFloat("Gravity.x", &gravity.x);
        ImGui::InputFloat("Gravity.y", &gravity.y);
        if (ImGui::Button("Flip Gravity")) {
            gravity = -gravity;
        }
        bool show = true;
        if (ImGui::CollapsingHeader("entities")) {
            ImGui::LabelText(util::to_string(entities.size()).c_str(), "entity count");
        }
        ImGui::End();
    }
}

sf::Vector2i World::get_mouse_position() {
    return sf::Mouse::getPosition(*window);
}

void World::add_entity(Entity *entity) {
    entity->set_world_ptr(this);
    entityAddQueue.push(entity);
}

int World::remove_entity(std::string tag) {
    int count = 0;
    size_t vectorLength = entities.size(); // size might change as we go through
    if (vectorLength > 0) {
        auto it = entities.begin();
        while (it != entities.end()) {
            if ((*it)->get_tag() == tag || tag == "") {
                Entity *ptr = (*it);
                it = entities.erase(it);
                delete ptr;
                count ++;
            }
            else ++it;
        }
    }
    return count;
}

void World::toggle_pause() {
    paused = !paused;
    if (paused) {
        for (auto &entity : entities) {
            entity->on_pause();
        }
    } else {
        for (auto &entity : entities) {
            entity->on_resume();
        }
    }
}
bool World::is_paused() { return paused; }

void World::on_notify(Event event, void *data) {
}

void World::disable_camera_clamp() {
    clamp = false;
}

void World::set_camera_clamp(sf::Vector2f topLeft, sf::Vector2f size) {
    clamp = true;
    clampPos = topLeft;
    clampSize = size;
}