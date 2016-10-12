#pragma once

// keyboard dude driver

#include "dude_driver.h"
#include "dude_entity.h"
#include "subject.h"

class KeyboardDriver : public DudeDriver, public Subject {
public:
    KeyboardDriver(EntityDude *dude);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
private:
    bool dragging; // is user dragging the mouse to select grenade direction & speed?
    sf::Vector2f dragStart;
};