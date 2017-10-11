#pragma once

// level complete state

#include "state.h"
#include "observer.h"
#include "subject.h"
#include <SFML/Graphics.hpp>

class World;

class StateWin : public State, public Observer, public Subject {
public:
    StateWin(World *world, int levelNum, bool *restartFlag, sf::Color clear);

    void on_event(sf::Event &event);
    void on_tick();
    void on_draw(sf::RenderWindow &window);
    void on_draw_ui(sf::RenderWindow &window);
    void on_gain_focus();
    void on_lose_focus();

    void on_notify(Event event, void *data);
    sf::Color get_clear_colour();

private:
    bool *restartFlag;
    int levelNum;
    sf::Color clear;
};
