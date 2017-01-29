#pragma once

// object that draws the hud

#include "observer.h"
#include <SFML/Graphics.hpp>

class Hud : public Observer{
public:
    Hud();

    void on_notify(Event event, void *data);

    void draw(sf::RenderWindow &window, sf::Vector2f camera);
private:
    // mouse & dragging
    bool dragging, moving;
    sf::Vector2f mouseDragStart;
    sf::Vector2f ballRestPos;
    sf::Texture txtArrow;
    sf::Texture txtCursorDrag;
    sf::Texture txtCursorStop;

    // wind
    sf::Vector2f currentWind;
};