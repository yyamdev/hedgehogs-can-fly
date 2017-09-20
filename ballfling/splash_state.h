#pragma once

// splash screen state

#include "state.h"
#include <SFML/Graphics.hpp>

class World;

class StateSplash : public State {
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
    int state;
    float alpha; // 0 to 1
    int mouseEventCount;
};
