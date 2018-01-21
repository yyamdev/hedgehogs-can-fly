#pragma once

/*
 * Options menu state.
 */

#include "SFGUI/Scale.hpp"
#include "SFGUI/CheckButton.hpp"
#include "state.h"
#include "observer.h"
#include "subject.h"

class World;

class StateOptions : public State, public Subject {
public:
    StateOptions(World *world, bool showEraseSave, sf::Color clear);

    void on_event(sf::Event &event);
    void on_tick();
    void on_draw(sf::RenderWindow &window);
    void on_gain_focus();
    void on_lose_focus();

    sf::Color get_clear_colour();

    void on_notify(Event event, void *data);

    std::shared_ptr<sfg::Scale> guiSliderMusic;
    std::shared_ptr<sfg::Scale> guiSliderSfx;
    std::shared_ptr<sfg::CheckButton> guiToggleTrail;
    std::shared_ptr<sfg::CheckButton> guiTogglePower;

private:
    bool showEraseSave;
    sf::Color clear;
};
