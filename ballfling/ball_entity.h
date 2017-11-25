#pragma once

// bouncy ball

#include "entity.h"
#include "subject.h"
#include "observer.h"
#include "terrain_entity.h"

class EntityTerrain;
class EntityEnemy;

extern const float BALL_TERM_VEL;
extern const float BALL_MAX_LAUNCH_SPEED;
extern const float BALL_MAX_LAUNCH_SPEED_NERF;
extern const float BALL_MAX_SPEED;


class EntityBall : public Entity, public Observer {
public:
    EntityBall();
    EntityBall(sf::Vector2f pos, sf::Vector2f vel, sf::Color colour, bool *restartFlag);
    ~EntityBall();

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

    void on_notify(Event event, void *data);
    bool is_at_rest();

    void bounce(float bounceFactor, sf::Vector2f norm);

    bool is_on_sand();

private:
    bool spawned_fireworks;
    bool *restartFlag;
    static sf::Texture txt;
    static sf::Texture txtPoint;
    static bool textureLoaded;
    sf::Sprite spr, sprPoint;
    sf::Color colour;
    sf::Vector2f mouse;
    float angle;
    float xDirection; //  1 >
                      // -1 <


    EntityTerrain *terrain;
    EntityEnemy *enemy;

    bool justSpawned;

    TerrainType lastTerrain;

    bool reactToInput;

    bool dead;
    sf::Clock deadTimer;

    sf::Vector2f lastContact;

    // physics
    sf::Vector2f contactPoint;
    sf::Clock clkRest;
    bool rest;
    sf::Vector2f prevRest;
    float prevAngle;
    void record_new_rest_pos();
    void stop_resting();
    void reset_to_rest();

    // moving
    bool dragging;
    sf::Vector2f dragStart;
    bool canFling;
    bool canNudge;
    sf::Clock clkFlingTimer;
    float nudgeStr;
    float maxFlingVelocity;

    sf::Clock clkWallTouch;
    bool touching_wall();
};
