#include "weapon_entity.h"
#include "build_options.h"
#include "terrain_entity.h"
#include <iostream>

EntityWeapon::EntityWeapon() {
    EntityWeapon(sf::Vector2f(0.f, 0.f), W_GRENADE);
}

EntityWeapon::EntityWeapon(sf::Vector2f pos, WeaponType weapon) : weapon(weapon) {
    position = pos;
    tag = "weapon";
    collisionRadius = 16.f;
    terrain = NULL;
}

void EntityWeapon::event(sf::Event &e) {
}

void EntityWeapon::tick(std::vector<Entity*> &entities) {
    position += velocity;

    // gravity
    sf::Vector2f gravity = sf::Vector2f(0.f, GRAVITY);
    float terminalVelocity = 6.f;
    velocity += gravity;
    if (velocity.y > terminalVelocity)
        velocity.y = terminalVelocity;

    // find terrain entity
    if (terrain == NULL) {
        for (Entity *e : entities) {
            if (e->get_tag() == "terrain") {
                terrain = (EntityTerrain*)e;
            }
        }
    }

    // collide with terrain & resolve
    if (terrain->intersects_with_circle(position, collisionRadius, NULL)) {
        position -= velocity;
        velocity.y = 0.f;
    }
}

void EntityWeapon::draw(sf::RenderWindow &window) {
    sf::CircleShape circle(collisionRadius);
    circle.setOrigin(collisionRadius, collisionRadius);
    circle.setPosition(position);
    window.draw(circle);
}

WeaponType EntityWeapon::get_weapon() {
    return weapon;
}