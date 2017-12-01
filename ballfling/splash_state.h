#pragma once

/*
 * Splash screen state shown at game startup.
 */

#include <SFML/Graphics.hpp>
#include "state.h"
#include "subject.h"

class World;

class StateSplash : public State, public Subject {
public:
    StateSplash(World *world);

    void on_event(sf::Event &event);
    void on_tick();
    void on_draw(sf::RenderWindow &window);
    void on_draw_ui(sf::RenderWindow &window);
    void on_gain_focus();
    void on_lose_focus();
private:
    sf::Texture txtSplash;
    sf::Sprite sprSplash;
    sf::Clock timer;
    int state = 0;
    float alpha = 0.f;
};
