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
    static bool textureLoaded;
    sf::Sprite spr;

    EntityTerrain *terrain;

    // physics
    float velocityAngular; // degrees per frame
    // returns normalized vector pointing down along the slope defined by a normal
    static sf::Vector2f get_slide_down(sf::Vector2f indicence, sf::Vector2f normal, sf::Vector2f gravity);

    bool dragging; // is user dragging the mouse to select grenade direction & speed?
    sf::Vector2f dragStart;
};