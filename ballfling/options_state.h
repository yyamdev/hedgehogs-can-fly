#pragma once

// game options state

#include "state.h"
#include "observer.h"
#include "subject.h"
#include "SFGUI/Scale.hpp"

class World;

class StateOptions : public State, public Observer, public Subject {
public:
    StateOptions(World *world);

    void on_event(sf::Event &event);
    void on_tick();
    void on_draw(sf::RenderWindow &window);
    void on_draw_ui(sf::RenderWindow &window);
    void on_gain_focus();
    void on_lose_focus();

    void on_notify(Event event, void *data);

    std::shared_ptr<sfg::Scale> guiSliderMusic;
    std::shared_ptr<sfg::Scale> guiSliderSfx;

private:
};