#pragma once

/*
 * Fireworks entity for end-of-level effect.
 * Also includes static method to create fireworks without ending the level.
 */

#include <SFML/System.hpp>
#include "entity.h"

class EntityFireworks : public Entity
{
public:
    EntityFireworks();
    EntityFireworks(sf::Vector2f pos);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

    static void do_fireworks(sf::Vector2f position, int count);
private:
    sf::Clock timer;
    int count = 0;
};
