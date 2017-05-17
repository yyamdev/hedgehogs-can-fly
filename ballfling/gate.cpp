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

sf::Vector2f solve_quadratic(float a, float b, float c) {
    float det = b * b - 4 * a * c;
    if (det < 0) {
        // complex roots
        return sf::Vector2f(0.f, 0.f);
    }

    sf::Vector2f result;
    result.x = (-b + sqrtf(det)) / 2.f * a;
    result.y = (-b - sqrtf(det)) / 2.f * a;

    return result;
}

bool Gate::intersects_circle(sf::Vector2f pos, float radius) {
    // determine if circle intersects line
    const float rtd = 3.14159f / 180.f;
    float adjustedAngle = angle - 90.f;
    sf::Vector2f circleToGate = position - pos;
    sf::Vector2f gateTangent = sf::Vector2f(cos(adjustedAngle * rtd), sin(adjustedAngle * rtd));
    float distanceToGate = fabs(util::dot(circleToGate, gateTangent));

    if (distanceToGate < radius) {
        // determine if circle intersects line segment
        float gradient = tanf(adjustedAngle * rtd);
        float yIntercept = position.y - gradient * position.x;
        float a = 1.f + gradient * gradient;
        float b = 2.f * ((gradient * yIntercept) - pos.x - (pos.y * gradient));
        float c = pos.x * pos.x + pos.y * pos.y + yIntercept * yIntercept - (radius * radius) - (2.f * pos.y * yIntercept);
        sf::Vector2f x = solve_quadratic(a, b, c);
        sf::Vector2f intersects[2];
        intersects[0].x = x.x;
        intersects[0].y = gradient * intersects[0].x + yIntercept;

        intersects[1].x = x.y;
        intersects[1].y = gradient * intersects[1].x + yIntercept;

        // drawing
        int1 = intersects[0];
        int2 = intersects[1];
    }

    // drawing
    circlePosDraw = pos;
    toGateDraw = circleToGate;
    tangentDraw = gateTangent;

    if (distanceToGate < radius) {
        return true;
    }
    else {
        return false;
    }
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

    draw_vector(circlePosDraw - world->camera, toGateDraw, util::len(toGateDraw), sf::Color::Blue, window);
    draw_vector(position - world->camera, tangentDraw, 64.f, sf::Color::Blue, window);
    sf::CircleShape p1(8.f);
    p1.setOrigin(sf::Vector2f(8.f, 8.f));
    p1.setFillColor(sf::Color::Green);
    p1.setPosition(int1 - world->camera);
    window.draw(p1);
    p1.setPosition(int2 - world->camera);
    window.draw(p1);
}