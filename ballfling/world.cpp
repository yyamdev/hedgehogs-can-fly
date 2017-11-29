#include "world.h"
#include <assert.h>
#include <iostream>
#include "subject.h"
#include "build_options.h"
#include "debug_draw.h"
#include "imgui.h"
#include "util.h"
#include "cursor.h"

World::World(sf::RenderWindow &window) {
    this->window = &window;
    paused = false;
    gravity = sf::Vector2f(0.f, GRAVITY);
    clamp = false;

    cameraFollowBall = false;
    smoothCamOn = false;
    smoothCam.maxSteps = 100;
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

    if (smoothCamOn) {
        camera += smoothCam.step;
        ++smoothCam.currentSteps;
        if (smoothCam.currentSteps > smoothCam.maxSteps) {
            smoothCamOn = false;
            set_cursor_visible(true);
        }
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

    if (edit && ImGui::CollapsingHeader("World")) {
        sf::Vector2f oldGrav = gravity;
        ImGui::InputFloat("Gravity.x", &gravity.x);
        ImGui::InputFloat("Gravity.y", &gravity.y);
        if (ImGui::Button("Flip Gravity")) {
            gravity = -gravity;
        }
        if (oldGrav != gravity) notify(EVENT_NEW_WORLD_GRAVITY, NULL);
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Smooth Cam")) {
            if (ImGui::Button("toggle camera follow")) {
                cameraFollowBall = !cameraFollowBall;
            }
            ImGui::SameLine();
            if (ImGui::Button("pan")) {
                camera = smoothCam.start;
                smoothCam.step = util::normalize(smoothCam.end - smoothCam.start) * smoothCam.speed;
                smoothCam.maxSteps = (int)(util::len(smoothCam.end - smoothCam.start) / util::len(smoothCam.step));
                smoothCam.currentSteps = 0;
                smoothCamOn = true;
                set_cursor_visible(false);
            }

            ImGui::InputFloat("Speed", &smoothCam.speed);

            if (ImGui::Button("set start")) {
                smoothCam.start = camera;
            }
            ImGui::SameLine();
            if (ImGui::Button("Jump to start")) {
                camera = smoothCam.start;
            }

            ImGui::Text("Start: %f, %f", smoothCam.start.x, smoothCam.start.y);

            if (ImGui::Button("set end")) {
                smoothCam.end = camera;
            }
            ImGui::SameLine();
            if (ImGui::Button("jump to end")) {
                camera = smoothCam.end;
            }

            ImGui::Text("End: %f, %f", smoothCam.end.x, smoothCam.end.y);
        }

        ImGui::Separator();
        ImGui::LabelText(util::to_string(entities.size()).c_str(), "entity count");
        if (ImGui::TreeNode("list")) {
            for (auto &e : entities) {
                if (ImGui::TreeNode(*e->get_tag().c_str() == 0 ? "_UNTAGGED_" : e->get_tag().c_str())) {
                    // posiiton
                    float pos[2] = {e->position.x, e->position.y};
                    ImGui::DragFloat2("position", pos);
                    e->position.x = pos[0];
                    e->position.y = pos[1];
                    // velocity
                    float vel[2] = {e->velocity.x, e->velocity.y};
                    ImGui::DragFloat2("velocity", vel);
                    e->velocity.x = vel[0];

                    e->velocity.y = vel[1];
                    ImGui::TreePop();
                }
            }
        ImGui::TreePop();
        }
    }
}

sf::Vector2i World::get_mouse_position() {
    return sf::Mouse::getPosition(*window);
}

void World::add_entity(Entity *entity) {
    entity->set_world_ptr(this);
    entityAddQueue.push(entity);
    entity->on_add();
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
    for (auto &entity : entities) {
        paused? entity->on_pause() : entity->on_resume();
    }
}

bool World::is_paused()
{
    return paused;
}

void World::set_pause(bool pausedNew)
{
    paused = pausedNew;
    for (auto &entity : entities) {
        paused? entity->on_pause() : entity->on_resume();
    }
}

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
