#pragma once

// Boost gate.

#include "entity.h"
#include <SFML/Graphics.hpp>

class Gate : public Entity {
public:
    Gate();
    Gate(sf::Vector2f position, float angle, float size, float strength);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

    bool intersects_circle(sf::Vector2f pos, float radius);

    sf::Vector2f position;
    float angle;
    float size;
    float strength;

    sf::Vector2f circlePosDraw, toGateDraw, tangentDraw, int1, int2;
};