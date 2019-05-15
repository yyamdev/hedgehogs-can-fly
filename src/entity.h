#pragma once

/*
 * Base class for an entity that is contained in a World.
 */

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "subject.h"

class World;
class EntityTerrain;

class Entity : public Subject {
public:
    Entity();
    virtual ~Entity(){}

    virtual void event(sf::Event &e) = 0;
    virtual void tick(std::vector<Entity*> &entities) = 0;
    virtual void draw(sf::RenderWindow &window) = 0;

    virtual void on_pause(){}
    virtual void on_resume(){}

    virtual void on_add(){}

    std::string get_tag();

    void set_world_ptr(World *world);

    bool intersects(const Entity &other);

    sf::Vector2f position;
    sf::Vector2f velocity;
    float collisionRadius = 0.f;
    bool remove = false;
protected:
    std::string tag = "";
    World *world = NULL;
private:
};
