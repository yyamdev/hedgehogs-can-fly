#include "tnt_entity.h"
#include <SFML/Graphics.hpp>
#include "world.h"
#include <iostream>
#include "util.h"

EntityTnt::EntityTnt() {
    EntityTnt(sf::Vector2f(0.f, 0.f));
}

EntityTnt::EntityTnt(sf::Vector2f pos) {
    position = pos;
    tag = "tnt";
    collisionRadius = 16.f;
}

void EntityTnt::event(sf::Event &e) {
}

void EntityTnt::tick(std::vector<Entity*> &entities) {
}

void EntityTnt::draw(sf::RenderWindow &window) {
    sf::RectangleShape rect(sf::Vector2f(32.f, 32.f));
    rect.setOrigin(sf::Vector2f(16.f, 16.f));
    rect.setFillColor(sf::Color::Red);
    rect.setPosition(position - world->camera);
    window.draw(rect);
}

void EntityTnt::touch() {
    std::cout << "TOUCH!\n";
}

bool EntityTnt::intersects_with_circle(sf::Vector2f pos, float rad) {
    // approximate circle to small bounding box
    float radSmall = rad - 4.f;
    sf::FloatRect obj(pos.x - radSmall, pos.y - radSmall, radSmall * 2.f, radSmall * 2.f);
    sf::FloatRect us(position.x - collisionRadius, position.y - collisionRadius, collisionRadius * 2.f, collisionRadius * 2.f);
    return obj.intersects(us);
}

sf::Vector2f EntityTnt::get_normal(sf::Vector2f pos, sf::Vector2f vel) {
    sf::Vector2f before = pos - vel;
    if (before.x < position.x - collisionRadius) return sf::Vector2f(-1.f, 0.f);
    if (before.x > position.x + collisionRadius) return sf::Vector2f( 1.f, 0.f);
    if (before.y < position.y - collisionRadius) return sf::Vector2f(0.f, -1.f);
    else return sf::Vector2f(0.f,  1.f);
}