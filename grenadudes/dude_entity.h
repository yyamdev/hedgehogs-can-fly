#pragma once

// a player or ai controllable character

#include "entity.h"
#include "dude_driver.h"
#include <memory>
#include "subject.h"
#include "build_options.h"
#include "weapons.h"

#define DUDE_SIZE 24.f
#define DUDE_COLLISION_PAD 0.f
#define DUDE_MAX_TERRAIN_CLIMB 6.f

#define PLAYER_NUMBER 1
#define AI_NUMBER 2

#define DUDE_SHOOT_COOLDOWN 2.f

#define DUDE_WEAPON_TIME 10.f

#define DUDE_TERM_VEL 6.f

class EntityTerrain;

struct DudeCollisionResponse {
    bool collision;
    bool collisionUnder;
    bool collisionAbove;
    float lowestY;
    float highestY;
};

class EntityDude : public Entity, public Observer {
public:
    EntityDude();
    EntityDude(sf::Vector2f pos, int number);

    void set_driver(std::unique_ptr<DudeDriver> driver);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

    enum EntityDudeDirection {
        DIRECTION_LEFT,
        DIRECTION_RIGHT
    };

    bool is_jumping();

    int get_number();
    void set_number(int number);

    WeaponType get_weapon();
    void set_weapon(WeaponType weapon);

    sf::Time get_cooldown_time();
    sf::Time get_weapon_cooldown_time();

    // control
    void move(EntityDudeDirection dir);
    void jump();
    void throw_grenade(sf::Vector2f dir, float speed);

    void on_notify(Event event, void *data);

private:
    DudeCollisionResponse intersecting_terrain(EntityTerrain *terrain, sf::Vector2f position);
    bool can_move(float xDelta, EntityTerrain *terrain);
    float get_y_on_terrain(sf::Vector2f position);

    std::unique_ptr<DudeDriver> driver;
    EntityTerrain *terrain;

    bool isJumping;
    int number;

    sf::Clock shootCooldownTimer;

    WeaponType weapon;
    sf::Clock weaponTimer;

    static sf::Texture txt;
    static bool textureLoaded;
    sf::Sprite spr;

    // physics constants
    const float terminalVelocity;
};