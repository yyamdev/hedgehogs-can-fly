#include "weapon_entity.h"
#include "build_options.h"
#include "terrain_entity.h"
#include <iostream>
#include "world.h"

sf::Texture EntityWeapon::txtSticky;
sf::Texture EntityWeapon::txtCluster;
bool EntityWeapon::texturesLoaded = false;

EntityWeapon::EntityWeapon() : terminalVelocity(WEAPON_TERM_VEL) {
    EntityWeapon(sf::Vector2f(0.f, 0.f), W_GRENADE);
}

EntityWeapon::EntityWeapon(sf::Vector2f pos, WeaponType weapon) : weapon(weapon), terminalVelocity(WEAPON_TERM_VEL) {
    position = pos;
    tag = "weapon";
    collisionRadius = 16.f;
    terrain = NULL;
    if (!texturesLoaded) {
        txtSticky.loadFromFile("data/sticky_pickup.png");
        txtCluster.loadFromFile("data/cluster_pickup.png");
        texturesLoaded = true;
    }
}

void EntityWeapon::event(sf::Event &e) {
}

void EntityWeapon::tick(std::vector<Entity*> &entities) {
    position += velocity;

    // gravity
    velocity += world->gravity;
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
    sf::Sprite spr(get_texture());
    spr.setOrigin(sf::Vector2f(collisionRadius, collisionRadius));
    spr.setPosition(position);
    window.draw(spr);
}

sf::Texture& EntityWeapon::get_texture() {
    if (weapon == W_STICKY_GRENADE) return txtSticky;
    if (weapon == W_CLUSTER_GRENADE) return txtCluster;
    return txtSticky; // todo -> return error texture (shouldn't get here)
}

WeaponType EntityWeapon::get_weapon() {
    return weapon;
}