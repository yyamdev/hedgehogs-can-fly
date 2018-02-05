#pragma once

/*
 * Object that draws the HUD overlay onto the ball.
 */

#include <SFML/Graphics.hpp>
#include "observer.h"

class EntityBall;

class Hud : public Observer{
public:
    Hud();
    void on_notify(Event event, void *data);
    void draw(sf::RenderWindow &window, sf::Vector2f camera, sf::Color levelColour);
private:
    bool dragging = false;
    EntityBall *ball = NULL;
    float power = 0.f, powerPrev = 0.f;

    sf::Vector2f mouseDragStart;

    sf::Texture txtArrow;
    sf::Texture txtNudge;
    sf::Sprite sprArrow;
    sf::Sprite sprNudge;
};
