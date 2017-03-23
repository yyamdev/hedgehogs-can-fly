#pragma once

// object that draws the hud

#include "observer.h"
#include <SFML/Graphics.hpp>

#define SHOW_INITIAL_INSTRUCTIONS 1

class Hud : public Observer{
public:
    Hud();

    void on_notify(Event event, void *data);

    void draw(sf::RenderWindow &window, sf::Vector2f camera);
private:
    // mouse & dragging
    bool dragging, canFling, canNudge;
    sf::Vector2f mouseDragStart;
    sf::Vector2f ballRestPos;
    sf::Texture txtArrow;
    sf::Texture txtBallNudge;
    int moveCount;
    sf::Font fntCounter;
    bool drawArrowOnBall;
    bool drawMoveCount;

    // wind
    sf::Vector2f currentWind;

    // instructions
    sf::Texture txtInstructionDrag, txtInstructionSpace;
    sf::Sprite sprInstructionDrag, sprInstructionSpace;
};