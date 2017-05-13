#pragma once

// generic entity that lives in a World

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
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

    bool intersects(const Entity &other);

    std::string get_tag();

    sf::Vector2f position;
    sf::Vector2f velocity;
    float collisionRadius;
    bool remove; // flag to remove entity from game

    void set_world_ptr(World *world);
protected:
    std::string tag;
    World *world;
private:
};