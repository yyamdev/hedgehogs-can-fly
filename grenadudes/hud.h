#pragma once

// object that draws the hud

#include "observer.h"
#include <SFML/Graphics.hpp>
#include "ai_driver.h"

struct HpBar {
    int hp;
    int maxHp;
};

class Hud : public Observer{
public:
    Hud();

    void on_notify(Event event, void *data);

    void draw(sf::RenderWindow &window);
private:
    // mouse dragging for throwing grenade
    bool dragging;
    sf::Vector2f mouseDragStart;

    // hp indicators
    HpBar hpPlayer;
    HpBar hpAi;

    // cooldown indicators
    float cooldownPlayer;
    float cooldownAi;

    // ai state
    AiState aiState;
    sf::Font fnt;
};