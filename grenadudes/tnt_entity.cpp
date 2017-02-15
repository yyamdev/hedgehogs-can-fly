#include "tnt_entity.h"
#include <SFML/Graphics.hpp>
#include "world.h"
#include <iostream>
#include "util.h"
#include "terrain_entity.h"

EntityTnt::EntityTnt() {
    EntityTnt(sf::Vector2f(0.f, 0.f));
}

EntityTnt::EntityTnt(sf::Vector2f pos) {
    position = pos;
    tag = "tnt";
    collisionRadius = 16.f;
    terrain = NULL;
    touched = false;
}

void EntityTnt::event(sf::Event &e) {
}

void EntityTnt::tick(std::vector<Entity*> &entities) {
    for (auto &e : entities) {
        if (e->get_tag() == "terrain")
            terrain = (EntityTerrain*)e;
    }

    if (touched && clkExplode.getElapsedTime().asSeconds() > 3.f) {
        if (terrain) {
            terrain->set_weak_terrain_circle(position, 128.f, false);
            remove = true;
        } else {
            std::cout << "TNT entity should have exploded but it didn't have a pointer to terrain entity\n";
        }
    }
}

void EntityTnt::draw(sf::RenderWindow &window) {
    sf::RectangleShape rect(sf::Vector2f(32.f, 32.f));
    rect.setOrigin(sf::Vector2f(16.f, 16.f));
    rect.setFillColor(sf::Color::Red);
    rect.setPosition(position - world->camera);
    window.draw(rect);
}

void EntityTnt::touch() {
    touched = true;
    clkExplode.restart();
}

bool EntityTnt::intersects_with_circle(sf::Vector2f pos, float rad) {
    // approximate circle to small bounding box
    float radSmall = rad - 2.f;
    sf::FloatRect obj(pos.x - radSmall, pos.y - radSmall, radSmall * 2.f, radSmall * 2.f);
    sf::FloatRect us(position.x - collisionRadius, position.y - collisionRadius, collisionRadius * 2.f, collisionRadius * 2.f);
    return obj.intersects(us);
}

sf::Vector2f EntityTnt::get_normal(sf::Vector2f pos, sf::Vector2f vel) {
    sf::Vector2f before = pos - vel;
    if (before.x < position.x - collisionRadius && vel.x > 0.f) return sf::Vector2f(-1.f, 0.f);
    if (before.x > position.x + collisionRadius && vel.x < 0.f) return sf::Vector2f( 1.f, 0.f);
    if (before.y < position.y - collisionRadius && vel.y > 0.f) return sf::Vector2f(0.f, -1.f);
    else return sf::Vector2f(0.f,  -1.f);
}