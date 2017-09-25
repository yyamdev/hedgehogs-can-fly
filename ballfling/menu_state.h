#pragma once

// main menu state

#include "state.h"
#include "observer.h"
#include "subject.h"
#include <SFML/Graphics.hpp>

class World;

class StateMenu : public State, public Observer, public Subject {
public:
    StateMenu(World *world);

    void on_event(sf::Event &event);
    void on_tick();
    void on_draw(sf::RenderWindow &window);
    void on_draw_ui(sf::RenderWindow &window);
    void on_gain_focus();
    void on_lose_focus();
    sf::Color get_clear_colour();

    void on_notify(Event event, void *data);

private:
    sf::Texture txtTitle;
    sf::Sprite sprTitle;
};
