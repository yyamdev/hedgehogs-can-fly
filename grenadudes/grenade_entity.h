#pragma once

// bouncy grenade

#include "entity.h"
#include "subject.h"

#define GRENADE_TERM_VEL 6.f

class EntityGrenade : public Entity {
public:
    EntityGrenade();
    EntityGrenade(sf::Vector2f pos, sf::Vector2f vel, bool sticky);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);
private:
    // physics
    const float terminalVelocity;

    bool sticky;
    bool stuck;
    bool stuckToEntity;
    Entity* stuckTo;
    sf::Vector2f stuckOffset;
    
    float bounciness;

    bool rest;
    sf::Clock restTimer;
    sf::Clock explodeTimer;
    bool collided;

    static sf::Texture txt;
    static bool textureLoaded;
    sf::Sprite spr;
    float angle;
};