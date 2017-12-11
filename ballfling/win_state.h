#pragma once

/*
 * Level complete screen.
 */

#include <SFML/Graphics.hpp>
#include "state.h"
#include "subject.h"

class World;

class StateWin : public State, public Subject {
public:
    StateWin(World *world, int levelNum, sf::Color clear, unsigned int millis);

    void on_event(sf::Event &event);
    void on_tick();
    void on_draw(sf::RenderWindow &window);
    void on_draw_ui(sf::RenderWindow &window);
    void on_gain_focus();
    void on_lose_focus();

    sf::Color get_clear_colour();

private:
    int levelNum;
    sf::Color clear;
    unsigned int millis;
};
