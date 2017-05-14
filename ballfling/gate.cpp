#include "gate.h"

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
}