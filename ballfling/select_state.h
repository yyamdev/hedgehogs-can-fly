#pragma once

// level select state

#include "state.h"
#include "observer.h"
#include "subject.h"

class World;

class StateSelect : public State, public Observer, public Subject {
public:
    StateSelect(World *world);

    void on_event(sf::Event &event);
    void on_tick();
    void on_draw(sf::RenderWindow &window);
    void on_draw_ui(sf::RenderWindow &window);
    void on_gain_focus();
    void on_lose_focus();

    void on_notify(Event event, void *data);

private:
};