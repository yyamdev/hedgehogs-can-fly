#include "gate.h"
#include <SFML/Graphics.hpp>
#include "world.h"
#include "util.h"
#include "debug_draw.h"

Gate::Gate() {
    Gate(sf::Vector2f(), 0.f, 0.f, 0.f);
}

Gate::Gate(sf::Vector2f position, float angle, float size, float strength) :
    position(position),
    angle(angle),
    size(size),
    strength(strength)
{
    tag = "gate";
}

void Gate::event(sf::Event &e) {}
void Gate::tick(std::vector<Entity*> &entities) {}

bool Gate::intersects_circle(sf::Vector2f pos, float radius) {
    // determine if circle intersects line
    const float rtd = 3.14159f / 180.f;
    float adjustedAngle = angle - 90.f;
    sf::Vector2f circleToGate = position - pos;
    sf::Vector2f gateTangent = sf::Vector2f(cos(adjustedAngle * rtd), sin(adjustedAngle * rtd));
    float distanceToGate = fabs(util::dot(circleToGate, gateTangent));

    if (distanceToGate < radius) {
        // determine if intersection is within gate bounds (horizontal size)
        float lenCircleToGate = util::len(circleToGate);
        float distanceFromCentre = sqrtf((lenCircleToGate * lenCircleToGate) - (distanceToGate * distanceToGate));
        if (distanceFromCentre < size / 2.f + radius) {
            return true;
        }
    }

    return false;
}

sf::Vector2f Gate::get_boost_vector() {
    const float rtd = 3.14159f / 180.f;
    float adjustedAngle = angle - 90.f;
    return strength * sf::Vector2f(cos(adjustedAngle * rtd), sin(adjustedAngle * rtd));
}

void Gate::draw(sf::RenderWindow &window) {
    sf::RectangleShape shape(sf::Vector2f(size, 4.f));
    shape.setOrigin(shape.getSize() / 2.f);
    shape.setPosition(position - world->camera);
    shape.setRotation(angle);
    shape.setFillColor(sf::Color::Red);
    window.draw(shape);

    sf::RectangleShape dir(sf::Vector2f(2.f, 32.f));
    dir.setOrigin(sf::Vector2f(1.f, 32.f));
    dir.setPosition(position - world->camera);
    dir.setRotation(angle);
    dir.setFillColor(sf::Color::Red);
    window.draw(dir);
}