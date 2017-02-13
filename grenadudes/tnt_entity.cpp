#include "tnt_entity.h"
#include <SFML/Graphics.hpp>
#include "world.h"

EntityTnt::EntityTnt() {
    EntityTnt(sf::Vector2f(0.f, 0.f));
}

EntityTnt::EntityTnt(sf::Vector2f pos) {
    position = pos;
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