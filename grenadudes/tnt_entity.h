#pragma once

// tnt box that can explode

#include "entity.h"

class EntityTnt : public Entity {
public:
    EntityTnt();
    EntityTnt(sf::Vector2f pos);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);
private:
};