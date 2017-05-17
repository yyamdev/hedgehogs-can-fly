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
#include "gate.h"

std::string level_num_to_filename(int levelNum);

class EntityTerrain;

class StatePlay : public State, public Observer, public Subject {
public:
    StatePlay(World *world, int levelNum);

    void on_event(sf::Event &event);
    void on_tick();
    void on_draw(sf::RenderWindow &window);
    void on_draw_ui(sf::RenderWindow &window);
    void on_gain_focus();
    void on_lose_focus();

    void on_notify(Event event, void *data);

private:
    std::string filename;
    sf::Color levelColour; // colour of ball and wall edges.

    EntityBall *player;

    Hud hud; // hud renderer
    Audio audio; // audio player
    EntityTerrain *terrain;

    bool completed;
    int levelNum;

    Gate gate;
};