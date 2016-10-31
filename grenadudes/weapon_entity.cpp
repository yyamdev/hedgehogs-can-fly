#include "weapon_entity.h"

EntityWeapon::EntityWeapon() {
    EntityWeapon(sf::Vector2f(0.f, 0.f), W_GRENADE);
}

EntityWeapon::EntityWeapon(sf::Vector2f pos, WeaponType weapon) : weapon(weapon) {
    position = pos;
    tag = "weapon";
    collisionRadius = 16.f;
}

void EntityWeapon::event(sf::Event &e) {
}

void EntityWeapon::tick(std::vector<Entity*> &entities) {
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