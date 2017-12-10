#pragma once

/*
 * End of game state.
 */

#include <SFML/Graphics.hpp>
#include "SFGUI/Button.hpp"
#include "state.h"
#include "subject.h"

class World;

class StateEnd : public State, public Subject {
public:
    StateEnd(World *world);

    void on_event(sf::Event &event);
    void on_tick();
    void on_draw(sf::RenderWindow &window);
    void on_draw_ui(sf::RenderWindow &window);
    void on_gain_focus();
    void on_lose_focus();

    sf::Color get_clear_colour();

    void create_gui();
private:
    sf::Clock timerFirework;
    sf::Clock timerState;

    float fireworkTime = 2.f;
    int state = 0;
    float nextFireworkSeconds;

    std::shared_ptr<sfg::Button> guiButtonSelect;
};
