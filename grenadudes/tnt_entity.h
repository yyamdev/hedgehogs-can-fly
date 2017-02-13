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

    bool intersects_with_circle(sf::Vector2f pos, float rad);
    sf::Vector2f get_normal(sf::Vector2f pos, sf::Vector2f vel);
    void touch();
private:
};