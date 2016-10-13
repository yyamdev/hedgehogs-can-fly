#include "dude_entity.h"
#include <iostream>
#include "build_options.h"
#include "terrain_entity.h"
#include "util.h"
#include "world.h"
#include "grenade_entity.h"

sf::Vector2f EntityDude::gravity;
float EntityDude::terminalVelocity;

sf::Texture EntityDude::txt;
bool EntityDude::textureLoaded = false;

EntityDude::EntityDude() {
    EntityDude(sf::Vector2f(0.f, 0.f), 0);
}

EntityDude::EntityDude(sf::Vector2f pos, int number) {
    this->position = pos;
    set_number(number);
    collisionRadius = DUDE_SIZE / 2.f + DUDE_COLLISION_PAD;
    tag = "dude";
    driver = NULL;
    gravity = sf::Vector2f(0.f, GRAVITY);
    terminalVelocity = 6.f;
    terrain = NULL;
    isJumping = false;
    hp = maxHp = START_HP_DUDE;
    invulnerable = false; // use hp
    notify(EVENT_DUDE_HP_CHANGE, (void*)this);
    number = 0;
    Subject::add_observer(this);
    if (!textureLoaded) {
        txt.loadFromFile("data/dude.png");
        textureLoaded = true;
    }
}

EntityDude::~EntityDude() {
    Subject::remove_observer(this);
}

void EntityDude::set_driver(std::unique_ptr<DudeDriver> driver) {
    this->driver = std::move(driver);
}

void EntityDude::event(sf::Event &e) {
    if (driver)
        driver->event(e);
}

void EntityDude::tick(std::vector<Entity*> &entities) {
    if (driver)
        driver->tick(entities);

    position += velocity;

    // gravity
    velocity += gravity;
    if (velocity.y > terminalVelocity)
        velocity.y = terminalVelocity;

    // loop through entities
    if (terrain == NULL) {
        for (Entity *e : entities) {
            if (e->get_tag() == "terrain") { // get terrain entity
                terrain = (EntityTerrain*)e;
            }
        }
    }

    // detect & handle collision with terrain
    DudeCollisionResponse collision;
    // repeatedly move down out of collision with terrain
    collision.collisionAbove = true;
    while (collision.collisionAbove) {
        collision = intersecting_terrain(terrain, position);
        if (collision.collisionAbove) {
            position.y += 1.f;
            velocity.y = 0.f;
        }
    }
    // repeatedly move up out of collision with terrain
    collision.collisionUnder = true;
    while (collision.collisionUnder) {
        collision = intersecting_terrain(terrain, position);
        if (collision.collisionUnder) {
            position.y -= 1.f;
            velocity.y = 0.f;
            isJumping = false;
        } else {
            position.y += 1.f;
        }
    }

    // don't move out of the world
    position.x = util::clamp(position.x, 0.f, (float)WINDOW_WIDTH);
    
    // die if we fall out of the world
    if (position.y > (float)WINDOW_HEIGHT) {
        remove = true;
    }

    // notify about cooldown change
    notify(EVENT_DUDE_COOLDOWN_CHANGE, (void*)this);
}

void EntityDude::draw(sf::RenderWindow &window) {
    spr.setTexture(txt);
    spr.setOrigin(spr.getLocalBounds().width / 2.f, spr.getLocalBounds().height / 2.f);
    spr.setPosition(position);
    window.draw(spr);

    if(driver)
        driver->draw(window);
}

DudeCollisionResponse EntityDude::intersecting_terrain(EntityTerrain *terrain, sf::Vector2f position) {
    bool collision = false;
    bool collisionUnder = false;
    bool collisionAbove = false;
    float lowestY = (float)WINDOW_HEIGHT; // 'highest' screen wise (-ve is higher)
    float highestY = 0.f; // 'lowest' screen wise (+ve is lower)
    for (float x = -DUDE_SIZE / 2.f; x<DUDE_SIZE / 2.f; ++x) {
        float xPos = x + position.x;
        float yPos1 = position.y + DUDE_SIZE / 2.f + 1.f; // under
        float yPos2 = position.y - DUDE_SIZE / 2.f + 1.f; // above
        if (terrain->get_solid(sf::Vector2f(xPos, yPos1))) {
            collision = true;
            collisionUnder = true;
            if (yPos1 < lowestY)
                lowestY = yPos1;
        }
        if (terrain->get_solid(sf::Vector2f(xPos, yPos2))) {
            collision = true;
            collisionAbove = true;
            if (yPos2 > highestY)
                highestY = yPos2;
        }
    }
    DudeCollisionResponse res;
    res.collision = collision;
    res.collisionUnder = collisionUnder;
    res.collisionAbove = collisionAbove;
    res.lowestY = lowestY;
    res.highestY = highestY;
    return res;
}

bool EntityDude::is_jumping() { return isJumping; }

sf::Time EntityDude::get_cooldown_time() { return shootCooldownTimer.getElapsedTime(); }

float EntityDude::get_y_on_terrain(sf::Vector2f position) { // find the new y at a specific point
    if (!terrain) return -1.f;
    sf::Vector2f newPos = position;
    DudeCollisionResponse collision;
    collision.collision = true;
    while (collision.collision) {
        collision = intersecting_terrain(terrain, newPos);
        if (collision.collision) {
            newPos.y -= 1.f;
        } else {
            newPos.y += 1.f;
        }
    }
    return newPos.y;
}

bool EntityDude::can_move(float xDelta, EntityTerrain *terrain) {
    sf::Vector2f newPos = position;
    newPos.x += xDelta;
    auto collision = intersecting_terrain(terrain, newPos);
    float delta = fabs(get_y_on_terrain(newPos) - newPos.y);
    if (!collision.collision || delta <= DUDE_MAX_TERRAIN_CLIMB)
        return true;
    else
        return false;
}

void EntityDude::move(EntityDudeDirection dir) {
    float delta = 2.f;
    if (dir == DIRECTION_LEFT && terrain && can_move(-delta, terrain))
        position.x -= delta;
    else if (dir == DIRECTION_RIGHT && terrain && can_move(delta, terrain))
        position.x += delta;
}

void EntityDude::jump() {
    if (!isJumping) {
        velocity.y = -4.f;
        isJumping = true;
    }
}

void EntityDude::throw_grenade(sf::Vector2f dir, float speed) {
    if (shootCooldownTimer.getElapsedTime().asSeconds() > DUDE_SHOOT_COOLDOWN) {
        speed = util::clamp(speed, 0.f, 10.f);
        sf::Vector2f start = position;
        start.x += util::sign(dir.x) * (DUDE_SIZE / 1.f);
        start.y -= DUDE_SIZE / 2.f;
        dir = dir / util::len(dir) * speed;
        world->add_entity(new EntityGrenade(start, dir, false));
        shootCooldownTimer.restart();
    }
}

int EntityDude::get_number(){ return number; }

void EntityDude::set_number(int number) {
    this->number = util::clamp(number, 0, 100);
}

void EntityDude::on_notify(Event event, void *data) {
    if (event == EVENT_ENTITY_HP_CHANGE) {
        notify(EVENT_DUDE_HP_CHANGE, (void*)this);
    }
}