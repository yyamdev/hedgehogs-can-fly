#pragma once

// gameplay state

#include "state.h"
#include "hud.h"
#include "observer.h"
#include <string>
#include "subject.h"
#include <SFML/Graphics.hpp>
#include "ball_entity.h"
#include "audio.h"

class EntityTerrain;

class StatePlay : public State, public Observer, public Subject {
public:
    StatePlay(World *world, std::string filename);

    void on_event(sf::Event &event);
    void on_tick();
    void on_draw(sf::RenderWindow &window);
    void on_draw_ui(sf::RenderWindow &window);
    void on_gain_focus();
    void on_lose_focus();

    void on_notify(Event event, void *data);

private:
    std::string filename;

    EntityBall *player;

    Hud hud; // hud renderer
    Audio audio; // audio player
    EntityTerrain *terrain;

    bool completed;
};