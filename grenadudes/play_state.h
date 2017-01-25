#pragma once

// gameplay state

#include "state.h"
#include "hud.h"
#include "observer.h"
#include <string>

class EntityTerrain;

class StatePlay : public State, public Observer, public Subject {
public:
    StatePlay(World *world, std::string filename);

    void on_event(sf::Event &event);
    void on_tick();
    void on_draw(sf::RenderWindow &window);
    void on_gain_focus();
    void on_lose_focus();

    void on_notify(Event event, void *data);

private:
    Hud hud; // hud renderer
    EntityTerrain *terrain;
    sf::Clock windClock;
};