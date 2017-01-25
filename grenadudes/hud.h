#pragma once

// object that draws the hud

#include "observer.h"
#include <SFML/Graphics.hpp>

class Hud : public Observer{
public:
    Hud();

    void on_notify(Event event, void *data);

    void draw(sf::RenderWindow &window);
private:
    // mouse dragging for throwing grenade
    bool dragging;
    sf::Vector2f mouseDragStart;

    // wind
    sf::Vector2f currentWind;
};