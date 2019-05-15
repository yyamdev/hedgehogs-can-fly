#pragma once

/*
 * Entity that just draws a sprite. Not affected by physics.
 */

#include <string>
#include <SFML/Graphics.hpp>
#include "entity.h"

class EntityBall;

class EntitySprite : public Entity {
public:
    EntitySprite(std::string filename, float x, float y);
    EntitySprite(std::string filename, float x, float y, float scale);
    EntitySprite(std::string filename, float x, float y, float scale, float angle);
    EntitySprite(std::string filename, float x, float y, float scale, float angle, bool fireworks);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

private:
    sf::Texture txt;
    sf::Sprite  spr;
    bool doFireworks = false;

    bool spawnedFireworks = false;
    EntityBall *ball;
};
