#pragma once

// bouncy ball

#include "entity.h"
#include "subject.h"
#include "observer.h"

#define BALL_TERM_VEL 9.f
#define BALL_MAX_LAUNCH_SPEED 14.f
#define BALL_MAX_SPEED 14.f
#define MIN_MOVE_SPEED 1.2f

class EntityTerrain;

class EntityBall : public Entity, public Observer {
public:
    EntityBall();
    EntityBall(sf::Vector2f pos, sf::Vector2f vel);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

    void on_notify(Event event, void *data);
    bool is_at_rest();

    void bounce(float bounceFactor, sf::Vector2f norm);
private:
    static sf::Texture txt;
    static sf::Texture txtPoint;
    static bool textureLoaded;
    sf::Sprite spr, sprPoint;

    EntityTerrain *terrain;

    // physics
    sf::Vector2f contactPoint;
    sf::Clock clkRest;
    bool rest;
    sf::Vector2f prevRest;

    bool dragging; // is user dragging the mouse to select grenade direction & speed?
    sf::Vector2f dragStart;
    bool canMove;
};