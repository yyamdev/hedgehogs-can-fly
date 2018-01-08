#include <iostream>
#include <assert.h>
#include "world.h"
#include "entity.h"
#include "subject.h"
#include "build_options.h"
#include "debug_draw.h"
#include "imgui.h"
#include "util.h"
#include "cursor.h"

World::World(sf::RenderWindow &window) : gravity(0.f, 0.15f), window(&window)
{
}

World::~World()
{
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

void World::event(sf::Event &e)
{
    for (auto &entity : entities) {
        entity->event(e);
    }
}

void World::tick()
{
    if (paused) return;

    size_t vectorLength = entities.size();
    if (vectorLength > 0) {
        auto it = entities.begin();
        while (it != entities.end()) {
            (*it)->tick(entities);

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

    if (smoothCam.active) {
        camera += smoothCam.step;
        ++smoothCam.currentSteps;
        if (smoothCam.currentSteps > smoothCam.maxSteps) {
            smoothCam.active = false;
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

void World::draw()
{
    for (auto &entity : entities) {
        entity->draw(*window);
    }

    // Debug menu
    if (edit && ImGui::CollapsingHeader("World")) {
        ImGui::Text("Smooth Camera Panning");

        if (ImGui::Button("start pan")) {
            camera = smoothCam.start;
            smoothCam.step = util::normalize(smoothCam.end - smoothCam.start) * smoothCam.speed;
            smoothCam.maxSteps = (int)(util::len(smoothCam.end - smoothCam.start) / util::len(smoothCam.step));
            smoothCam.currentSteps = 0;
            smoothCam.active = true;
            set_cursor_visible(false);
        }
        ImGui::InputFloat("Pan Speed", &smoothCam.speed);

        ImGui::Text("Start Position: %f, %f", smoothCam.start.x, smoothCam.start.y);
        if (ImGui::Button("set start")) {
            smoothCam.start = camera;
        }
        ImGui::SameLine();
        if (ImGui::Button("Jump to start")) {
            camera = smoothCam.start;
        }

        ImGui::Text("End Position: %f, %f", smoothCam.end.x, smoothCam.end.y);
        if (ImGui::Button("set end")) {
            smoothCam.end = camera;
        }
        ImGui::SameLine();
        if (ImGui::Button("jump to end")) {
            camera = smoothCam.end;
        }

        ImGui::Separator();

        ImGui::LabelText(util::to_string(entities.size()).c_str(), "entity count");
        if (ImGui::TreeNode("list")) {
            for (auto &e : entities) {
                if (ImGui::TreeNode(*e->get_tag().c_str() == 0 ? "UNTAGGED" : e->get_tag().c_str())) {
                    float pos[2] = {e->position.x, e->position.y};
                    ImGui::DragFloat2("position", pos);
                    e->position.x = pos[0];
                    e->position.y = pos[1];

                    float vel[2] = {e->velocity.x, e->velocity.y};
                    ImGui::DragFloat2("velocity", vel);
                    e->velocity.x = vel[0];
                    e->velocity.y = vel[1];

                    if (ImGui::Button("remove")) {
                        e->remove = true;
                    }
                    ImGui::TreePop();
                }
            }
        ImGui::TreePop();
        }
    }
}

void World::add_entity(Entity *entity)
{
    entity->set_world_ptr(this);
    entityAddQueue.push(entity);
    entity->on_add();
}

int World::remove_entity(std::string tag)
{
    int count = 0;
    size_t vectorLength = entities.size();

    if (vectorLength > 0) {
        auto it = entities.begin();
        while (it != entities.end()) {
            if ((*it)->get_tag() == tag || tag == ENTITY_TAG_ALL) {
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

void World::toggle_pause()
{
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

void World::disable_camera_clamp()
{
    clamp = false;
}

void World::set_camera_clamp(sf::Vector2f topLeft, sf::Vector2f size)
{
    clamp = true;
    clampPos = topLeft;
    clampSize = size;
}
