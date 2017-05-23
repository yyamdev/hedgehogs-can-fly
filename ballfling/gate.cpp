#include "gate.h"
#include <SFML/Graphics.hpp>
#include "world.h"
#include "util.h"
#include "debug_draw.h"
#include "particle.h"
#include <iostream>

EntityGate::EntityGate() {
    EntityGate(sf::Vector2f(), 0.f, 0.f, 0.f, sf::Color::White);
}

EntityGate::EntityGate(sf::Vector2f position, float angle, float size, float strength, sf::Color colour) :
    position(position),
    angle(angle),
    size(size),
    strength(strength),
    colour(colour),
    selected(false)
{
    tag = "gate";
}

void EntityGate::event(sf::Event &e) {
    if (edit && e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        float rad = size / 2.f;
        float mouseX = (float)e.mouseButton.x + world->camera.x;
        float mouseY = (float)e.mouseButton.y + world->camera.y;
        if (mouseX > position.x - rad &&mouseX < position.x + rad &&
            mouseY > position.y - rad && mouseY < position.y + rad) {
            selected = !selected;
        }
    }

    if (edit && e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::A) {
        selected = true;
    }

    if (edit && e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::D) {
        selected = false;
    }
}
void EntityGate::tick(std::vector<Entity*> &entities) {}

bool EntityGate::intersects_circle(sf::Vector2f pos, float radius) {
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

sf::Vector2f EntityGate::get_boost_vector() {
    const float rtd = 3.14159f / 180.f;
    float adjustedAngle = angle - 90.f;
    return strength * sf::Vector2f(cos(adjustedAngle * rtd), sin(adjustedAngle * rtd));
}

sf::Vector2f EntityGate::get_boost_vector_normalised() {
    const float rtd = 3.14159f / 180.f;
    float adjustedAngle = angle - 90.f;
    return sf::Vector2f(cos(adjustedAngle * rtd), sin(adjustedAngle * rtd));
}

void EntityGate::draw(sf::RenderWindow &window) {
    // Get vector down the line
    const float rtd = 3.14159f / 180.f;
    sf::Vector2f gateLine = sf::Vector2f(cos(angle * rtd), sin(angle * rtd));
    int particlesPerFrame = 8;
    while (particlesPerFrame > 0) {
        --particlesPerFrame;
        float step = util::rnd(0.f, size);
        float dist = 1.f;
        float accDist = .4f;
        add_particle(
            (position - gateLine * size / 2.f) + step * gateLine,
            get_boost_vector_normalised() * 4.f + sf::Vector2f(util::rnd(-dist, dist), util::rnd(-dist, dist)),
            sf::Vector2f(util::rnd(-accDist, accDist), util::rnd(-accDist, accDist)),
            sf::Color::White,
            16 );
    }
    
    sf::Color col = colour;
    if (selected) col = sf::Color::Red;
    sf::RectangleShape shape(sf::Vector2f(size, 2.f));
    shape.setOrigin(shape.getSize() / 2.f);
    shape.setPosition(position - world->camera);
    shape.setRotation(angle);
    shape.setFillColor(col);
    window.draw(shape);
}