#pragma once

/*
 * Container class to hold and manage Entities.
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <string>
#include "observer.h"
#include "events.h"
#include "subject.h"

#define ENTITY_TAG_ALL ""

class Entity;

class World : public Subject {
public:
    World(sf::RenderWindow &window);
    ~World();

    void event(sf::Event &e);
    void tick();
    void draw();

    void add_entity(Entity *entity);
    int remove_entity(std::string tag); // Returns number of entities removed

    void toggle_pause();
    bool is_paused();
    void set_pause(bool pausedNew);

    void set_camera_clamp(sf::Vector2f topLeft, sf::Vector2f size);
    void disable_camera_clamp();
    sf::Vector2f camera, clampPos, clampSize;

    sf::Vector2f gravity;

private:
    sf::RenderWindow *window;
    std::vector<Entity*> entities;
    std::queue<Entity*> entityAddQueue;

    bool paused = false;
    bool clamp = false;

    struct {
        sf::Vector2f start;
        sf::Vector2f end;

        float speed;
        sf::Vector2f step;
        int currentSteps;
        int maxSteps = 100;

        bool active = false;
    } smoothCam;
};
