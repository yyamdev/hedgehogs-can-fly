#pragma once

/*
 * Player entity.
 */

#include "entity.h"
#include "subject.h"
#include "observer.h"
#include "terrain_entity.h"

class EntityTerrain;

extern const float BALL_TERM_VEL;
extern const float BALL_MAX_LAUNCH_SPEED;
extern const float BALL_MAX_LAUNCH_SPEED_NERF;
extern const float BALL_MAX_SPEED;

class EntityBall : public Entity {
public:
    EntityBall();
    EntityBall(sf::Vector2f pos, sf::Vector2f vel, sf::Color colour);
    ~EntityBall();

    void on_add();

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

    bool is_at_rest();

    void bounce(float bounceFactor, sf::Vector2f norm);

    bool is_on_sand();
    bool can_fling();
    bool can_nudge();

private:
    bool spawned_fireworks = false;
    bool *restartFlag;
    static sf::Texture txt;
    static sf::Texture txtPoint;
    static bool textureLoaded;
    sf::Sprite spr, sprPoint;
    sf::Color colour;
    sf::Vector2f mouse;
    float angle = 0.f;
    float xDirection; //  1 >
                      // -1 <


    EntityTerrain *terrain = NULL;

    bool moveCamera = true;

    bool justSpawned = false;

    TerrainType lastTerrain, terrainFlungOn;

    bool reactToInput = true;

    bool dead = false;
    sf::Clock deadTimer;

    sf::Vector2f lastContact;

    // physics
    sf::Clock clkRest;
    bool rest = false;
    sf::Vector2f prevRest;
    float prevAngle;
    void record_new_rest_pos();
    void stop_resting();
    void reset_to_rest();

    // moving
    bool dragging = false;
    sf::Vector2f dragStart;
    bool canFling = false;
    bool canNudge = true;
    sf::Clock clkFlingTimer;
    float nudgeStr = .04f;
    float maxFlingVelocity = 1.2f; // if ball is faster than this, you can't start a fling

    sf::Clock clkWallTouch;
    bool touching_wall();
};
