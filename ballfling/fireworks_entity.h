#pragma once

#include "entity.h"
#include <SFML/System.hpp>
#include "subject.h"

class EntityFireworks : public Entity
{
public:
    EntityFireworks();
    EntityFireworks(sf::Vector2f pos);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);
private:
    sf::Clock timer;
    int count;
};
