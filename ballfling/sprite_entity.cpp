#include "sprite_entity.h"
#include <string>
#include <SFML/Graphics.hpp>
#include "world.h"

EntitySprite::EntitySprite(std::string filename, float x, float y) {
    txt.loadFromFile(filename);
    spr.setTexture(txt);
    spr.setOrigin(txt.getSize().x / 2.f, txt.getSize().y / 2.f);
    position.x = x;
    position.y = y;
    spr.setPosition(x, y);
    tag = filename;
};

void EntitySprite::event(sf::Event &e) {}

void EntitySprite::tick(std::vector<Entity*> &entities) {
    spr.setPosition(position.x, position.y);
}

void EntitySprite::draw(sf::RenderWindow &window) {
    spr.move(-world->camera);
    window.draw(spr);
    spr.move(world->camera);
}
