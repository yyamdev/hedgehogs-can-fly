#pragma once

// powerup spawner entity

#include "entity.h"
#include <SFML/Graphics.hpp>

class EntitySpawner : public Entity {
public:
    EntitySpawner();

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);
private:
    sf::Clock spawnTimer;
    float nextSpawn; // seconds
    float get_next_spawn_time();
};