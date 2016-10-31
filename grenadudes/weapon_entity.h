#pragma once

// weapon pickup

#include "entity.h"
#include <SFML/Graphics.hpp>
#include "weapons.h"

class EntityTerrain;

class EntityWeapon : public Entity {
public:
    EntityWeapon();
    EntityWeapon(sf::Vector2f pos, WeaponType weapon);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

    WeaponType get_weapon();
private:
    WeaponType weapon;
    EntityTerrain *terrain;
};