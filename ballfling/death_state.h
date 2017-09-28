#pragma once

// death state

#include "state.h"
#include "observer.h"
#include "subject.h"

class World;

class StateDeath : public State, public Observer, public Subject {
public:
    StateDeath(World *world, bool *restartFlag);

    void on_event(sf::Event &event);
    void on_tick();
    void on_draw(sf::RenderWindow &window);
    void on_draw_ui(sf::RenderWindow &window);
    void on_gain_focus();
    void on_lose_focus();

    void on_notify(Event event, void *data);

private:
    bool *restartFlag;
};
