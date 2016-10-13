#pragma once

// bouncy grenade

#include "entity.h"
#include "subject.h"

class EntityGrenade : public Entity {
public:
    EntityGrenade();
    EntityGrenade(sf::Vector2f pos, sf::Vector2f vel, bool sticky);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);
private:
    // physics
    static sf::Vector2f gravity;
    static float terminalVelocity;

    bool sticky;
    bool stuck;
    bool stuckToPlayer;
    sf::Vector2f playerToGrenadeStuck;
    Entity* stuckTo;

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