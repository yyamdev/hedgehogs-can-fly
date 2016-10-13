#pragma once

// circular explosion, only lasts a few moments. damages other entities around it

#include "entity.h"
#include <SFML/Graphics.hpp>

#define EXPLOSION_DEFORMATION_RADIUS 32.f
#define EXPLOSION_RADIUS 64.f
#define EXPLOSION_MILLISECONDS_ALIVE 100

class EntityExplosion : public Entity {
public:
    EntityExplosion();
    EntityExplosion(sf::Vector2f pos);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);
private:
    sf::Clock explosionTime;
    bool justCreated;
};