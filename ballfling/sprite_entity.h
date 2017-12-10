#pragma once

// sprite entity

#include "entity.h"
#include <string>
#include <SFML/Graphics.hpp>

class EntitySprite : public Entity {
public:
    EntitySprite(std::string filename, float x, float y);
    EntitySprite(std::string filename, float x, float y, float scale);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

private:
    sf::Texture txt;
    sf::Sprite  spr;
};
