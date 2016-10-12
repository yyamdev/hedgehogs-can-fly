#pragma once

// generic abstract base class for an object that controls an EntityDude

#include <SFML/Graphics.hpp>
#include "entity.h"

class EntityDude;

class DudeDriver{
public:
    inline DudeDriver(EntityDude *dude) {
        this->dude = dude;
    }
    virtual ~DudeDriver(){}

    virtual void event(sf::Event &e) = 0;
    virtual void tick(std::vector<Entity*> &entities) = 0;
    virtual void draw(sf::RenderWindow &window){}
protected:
    EntityDude *dude;
};