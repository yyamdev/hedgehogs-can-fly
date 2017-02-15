#include "tnt_entity.h"
#include <SFML/Graphics.hpp>
#include "world.h"
#include <iostream>
#include "util.h"
#include "terrain_entity.h"

sf::Texture EntityTnt::txt;
bool EntityTnt::textureLoaded = false;

EntityTnt::EntityTnt() {
    EntityTnt(sf::Vector2f(0.f, 0.f));
}

EntityTnt::EntityTnt(sf::Vector2f pos) {
    position = pos;
    tag = "tnt";
    collisionRadius = 16.f;
    terrain = NULL;
    touched = false;
    pulse = false;
    pulses = 0;
    if (!textureLoaded) {
        textureLoaded = true;
        txt.loadFromFile("data/tnt.png");
    }
}

void EntityTnt::event(sf::Event &e) {
}

void EntityTnt::tick(std::vector<Entity*> &entities) {
    for (auto &e : entities) {
        if (e->get_tag() == "terrain")
            terrain = (EntityTerrain*)e;
    }

    if (touched && clkPulse.getElapsedTime().asSeconds() > 1.5f) {
        clkPulse.restart();
        pulse = true;
        ++pulses;
        // TODO -> play sound pulse
        if (pulses >= 4) {
            if (terrain) {
                // TODO -> play sound explode
                terrain->set_weak_terrain_circle(position, 96.f, false);
                world->add_entity(new EntityExplosionGfx(position, 96.f));
                remove = true;
            } else
                std::cout << "TNT entity should have exploded but it didn't have a pointer to terrain entity\n";
        }
    }
    else if (touched && clkPulse.getElapsedTime().asSeconds() > 0.4f) {
        pulse = false;
    }
}

void EntityTnt::draw(sf::RenderWindow &window) {
    sf::Sprite spr(txt);
    spr.setOrigin(sf::Vector2f((float)txt.getSize().x / 2.f, (float)txt.getSize().y / 2.f));
    spr.setPosition(position - world->camera);
    if (!pulse) spr.setColor(sf::Color(210,210,210));
    window.draw(spr);
}

void EntityTnt::touch() {
    touched = true;
    pulses = 0;
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


EntityExplosionGfx::EntityExplosionGfx() {
    EntityExplosionGfx(sf::Vector2f(0.f, 0.f), 0.f);
}

EntityExplosionGfx::EntityExplosionGfx(sf::Vector2f pos, float rad) {
    life.restart();
    position = pos;
    this->rad = rad;
}

void EntityExplosionGfx::event(sf::Event &e) {}

void EntityExplosionGfx::tick(std::vector<Entity*> &entities) {
    if (life.getElapsedTime().asSeconds() > 0.1f) {
        remove = true;
    }
}

void EntityExplosionGfx::draw(sf::RenderWindow &window) {
    sf::CircleShape c;
    c.setRadius(rad);
    c.setOrigin(sf::Vector2f(rad, rad));
    c.setPosition(position - world->camera);
    c.setFillColor(sf::Color::White);
    window.draw(c);
}