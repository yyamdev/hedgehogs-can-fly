#pragma once

// world with entities

#include <SFML/Graphics.hpp>
#include "entity.h"
#include <vector>
#include <queue>
#include <string>
#include "observer.h"
#include "events.h"
#include "subject.h"

#define ENTITY_TAG_ALL ""

class World : public Observer, public Subject{
public:
    World(sf::RenderWindow &window);
    ~World();

    void on_notify(Event event, void *data);

    void event(sf::Event &e);
    void tick();
    void draw();

    sf::Vector2i get_mouse_position();

    void add_entity(Entity *entity);
    int remove_entity(std::string tag); // returns number of entities removed
    void toggle_pause();

    sf::Vector2f gravity;
private:
    sf::RenderWindow *window;
    std::vector<Entity*> entities;
    std::queue<Entity*> entityAddQueue;
    bool paused;
};