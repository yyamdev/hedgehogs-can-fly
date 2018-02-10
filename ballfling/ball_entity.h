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
    bool is_on_sand();
    bool can_fling();

private:
    void record_new_rest_pos();
    void stop_resting();
    void reset_to_rest();
    bool touching_wall();

    bool moveCamera = true;
    bool reactToInput = true;
    bool justSpawned = false;
    bool spawned_fireworks = false;
    bool rest = false;
    bool dragging = false;
    bool canFling = false;

    float angle = 0.f, prevAngle = 0.f;
    float xDirection;
    float nudgeStr = .04f;
    float maxFlingVelocity = 1.2f;

    sf::Vector2f mouse;
    sf::Vector2f lastContact;
    sf::Vector2f prevRest;
    sf::Vector2f dragStart;

    sf::Clock clkRest;
    sf::Clock clkFlingTimer;
    sf::Clock clkWallTouch;

    sf::Sprite spr;
    sf::Color colour;
    EntityTerrain *terrain = NULL;
    TerrainType lastTerrain, terrainFlungOn;
};
