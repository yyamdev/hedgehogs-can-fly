#pragma once

// Boost gate.

#include "entity.h"
#include <SFML/Graphics.hpp>

class EntityGate : public Entity {
public:
    EntityGate();
    EntityGate(sf::Vector2f position, float angle, float size, float strength, sf::Color colour);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

    bool intersects_circle(sf::Vector2f pos, float radius);

    sf::Vector2f get_boost_vector();
    sf::Vector2f get_boost_vector_normalised();

    sf::Vector2f position;
    float angle;
    float size;
    float strength;
    sf::Color colour;
    bool selected;
};