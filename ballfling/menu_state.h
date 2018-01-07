#pragma once

/*
 * Main menu state.
 */

#include <SFML/Graphics.hpp>
#include "state.h"
#include "observer.h"
#include "subject.h"

class World;

class StateMenu : public State, public Subject {
public:
    StateMenu(World *world);

    void on_event(sf::Event &event);
    void on_tick();
    void on_draw(sf::RenderWindow &window);
    void on_gain_focus();

private:
    sf::Texture txtTitle, txtHog;
    sf::Sprite sprTitle, sprHog;
};
