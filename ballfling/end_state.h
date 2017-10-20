#pragma once

// end game state

#include "state.h"
#include "observer.h"
#include "subject.h"
#include <SFML/Graphics.hpp>

class World;

class StateEnd : public State, public Observer, public Subject {
public:
    StateEnd(World *world);

    void on_event(sf::Event &event);
    void on_tick();
    void on_draw(sf::RenderWindow &window);
    void on_draw_ui(sf::RenderWindow &window);
    void on_gain_focus();
    void on_lose_focus();

    void on_notify(Event event, void *data);
    sf::Color get_clear_colour();

    void create_gui();
private:
    sf::Clock timerFirework;
    float fireworkTime;
    sf::Clock timerState;

    int state;
};
