#pragma once

// bouncy ball

#include "entity.h"
#include "subject.h"

#define BALL_TERM_VEL 6.f

class EntityTerrain;

class EntityBall : public Entity {
public:
    EntityBall();
    EntityBall(sf::Vector2f pos, sf::Vector2f vel);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);
private:
    static sf::Texture txt;
    static sf::Texture txtPoint;
    static bool textureLoaded;
    sf::Sprite spr, sprPoint;

    EntityTerrain *terrain;

    // physics
    bool disable;
    sf::Vector2f contactPoint;
    sf::Vector2f contactPoint2;
    sf::Vector2f contactPoint3;
    sf::Clock clkRest;
    bool rest;

    bool dragging; // is user dragging the mouse to select grenade direction & speed?
    sf::Vector2f dragStart;

    int cameraPan;
};