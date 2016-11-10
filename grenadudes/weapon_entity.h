#pragma once

// weapon pickup

#include "entity.h"
#include <SFML/Graphics.hpp>
#include "weapons.h"

#define WEAPON_TERM_VEL 6.f

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
    static sf::Texture txtSticky;
    static sf::Texture txtCluster;
    static bool texturesLoaded;
    sf::Texture& get_texture();

    WeaponType weapon;
    EntityTerrain *terrain;
    const float terminalVelocity;
};