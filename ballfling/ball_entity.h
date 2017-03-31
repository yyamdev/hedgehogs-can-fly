#pragma once

// bouncy ball

#include "entity.h"
#include "subject.h"
#include "observer.h"

#define BALL_TERM_VEL 9.f
#define BALL_MAX_LAUNCH_SPEED 14.f
#define BALL_MAX_SPEED 14.f

class EntityTerrain;

class EntityBall : public Entity, public Observer {
public:
    EntityBall();
    EntityBall(sf::Vector2f pos, sf::Vector2f vel);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

    void on_notify(Event event, void *data);
    bool is_at_rest();

    void bounce(float bounceFactor, sf::Vector2f norm);
private:
    static sf::Texture txt;
    static sf::Texture txtPoint;
    static bool textureLoaded;
    sf::Sprite spr, sprPoint;
    float angle;
    sf::Vector2f mouse;
    bool resetEnabled;

    EntityTerrain *terrain;

    bool reactToInput;

    // physics
    sf::Vector2f contactPoint;
    sf::Clock clkRest;
    bool rest;
    sf::Vector2f prevRest;
    void record_new_rest_pos();
    void stop_resting();
    void reset_to_rest();

    // moving
    bool dragging;
    sf::Vector2f dragStart;
    bool canFling;
    bool canNudge;
    enum DragMode {
        DM_REST = 0,
        DM_TIME = 1,
        DM_NUDGE = 2,
    } dragMode;
    int flingTimerValue;
    sf::Clock clkFlingTimer;
    float nudgeStr;
    float maxFlingVelocity;
    
    sf::Clock clkWallTouch;
    bool touching_wall();
};