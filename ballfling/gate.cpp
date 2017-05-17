#include "gate.h"
#include <SFML/Graphics.hpp>

Gate::Gate() {
    Gate(sf::Vector2f(), 0.f, 0.f, 0.f);
}

Gate::Gate(sf::Vector2f position, float angle, float size, float strength) :
    position(position),
    angle(angle),
    size(size),
    strength(strength)
{
}

void Gate::event(sf::Event &e) {}
void Gate::tick(std::vector<Entity*> &entities) {}

bool Gate::intersects_circle(sf::Vector2f position, float radius) {
    return false;
}

void Gate::draw(sf::RenderWindow &window) {
    sf::RectangleShape shape(sf::Vector2f(size, 4.f));
    shape.setOrigin(shape.getSize() / 2.f);
    shape.setPosition(position);
    shape.setRotation(angle);
    shape.setFillColor(sf::Color::Red);
    window.draw(shape);

    sf::RectangleShape dir(sf::Vector2f(2.f, 32.f));
    dir.setOrigin(sf::Vector2f(1.f, 32.f));
    dir.setPosition(position);
    dir.setRotation(angle);
    dir.setFillColor(sf::Color::Red);
    window.draw(dir);
}