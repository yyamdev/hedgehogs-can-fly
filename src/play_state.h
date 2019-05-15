#pragma once

/*
 * Gameplay state.
 */

#include <string>
#include <SFML/Graphics.hpp>
#include "state.h"
#include "hud.h"
#include "observer.h"
#include "subject.h"

class EntityTerrain;
class EntityBall;
class EntityTimer;

class StatePlay : public State, public Observer, public Subject {
public:
    StatePlay(World *world, int levelNum);
    sf::Color get_clear_colour();

    void on_event(sf::Event &event);
    void on_tick();
    void on_draw(sf::RenderWindow &window);
    void on_draw_ui(sf::RenderWindow &window);
    void on_gain_focus();
    void on_lose_focus();

    void on_notify(Event event, void *data);

private:
    bool initSuccess = false;
    sf::Color levelColour; // Colour of wall edges
    sf::Color backgroundColor; // Clear colour
    int levelNum;

    EntityTerrain *terrain;
    EntityBall *player;
    EntityTimer *timer;

    Hud hud;

    bool restartOnResume = false;
};
