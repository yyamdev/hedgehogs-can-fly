#pragma once

// generic entity that lives in a World

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "subject.h"

#define ENTITY_DAMAGE_COOLDOWN_MILLISECONDS 1000

class World;

class Entity : public Subject {
public:
    Entity();
    virtual ~Entity(){}

    virtual void event(sf::Event &e) = 0;
    virtual void tick(std::vector<Entity*> &entities) = 0;
    virtual void draw(sf::RenderWindow &window) = 0;

    bool intersects(Entity &other);

    std::string get_tag();

    int get_hp();
    int get_max_hp();
    void set_hp(int hp);
    void damage(int dmg);

    sf::Vector2f position;
    sf::Vector2f velocity;
    float collisionRadius;
    bool remove; // flag to remove entity from game

    void set_world_ptr(World *world);
protected:
    // helper methods
    static sf::Vector2f reflect(const sf::Vector2f incident, const sf::Vector2f unitNormal);
    static sf::Vector2f apply_friction(const sf::Vector2f vector, const float friction);
    static float dot(const sf::Vector2f a, const sf::Vector2f b);

    std::string tag;
    World *world;
    int hp;
    int maxHp;
    bool invulnerable;

    sf::Clock clockDamageCooldown;
private:
};