#include "grenade_entity.h"
#include "terrain_entity.h"
#include "util.h"
#include <vector>
#include <tuple>
#include "build_options.h"
#include "explosion_entity.h"
#include "world.h"

sf::Texture EntityGrenade::txt;
sf::Texture EntityGrenade::txtSticky;
sf::Texture EntityGrenade::txtCluster;
bool EntityGrenade::textureLoaded = false;

EntityGrenade::EntityGrenade() : terminalVelocity(GRENADE_TERM_VEL) {
    EntityGrenade(sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 0.f), false, false);
}

EntityGrenade::EntityGrenade(sf::Vector2f pos, sf::Vector2f vel, bool sticky, bool cluster) : terminalVelocity(GRENADE_TERM_VEL) {
    this->position = pos;
    this->velocity = vel;
    this->sticky = sticky;
    this->cluster = cluster;
    stuck = false;
    stuckToEntity = false;
    tag = "grenade";
    collisionRadius = 8.f;
    rest = false;
    explodeTimer.restart();
    collided = false;
    bounciness = 0.f;
    invulnerable = false;
    hp = maxHp = START_HP_GRENADE;
    if (!textureLoaded) {
        txt.loadFromFile("data/grenade.png");
        txtSticky.loadFromFile("data/sticky.png");
        txtCluster.loadFromFile("data/cluster.png");
        textureLoaded = true;
    }
    angle = 0.f;
}

void EntityGrenade::event(sf::Event &e) {
}

void EntityGrenade::tick(std::vector<Entity*> &entities) {
    sf::Vector2f oldPos = position;
    // apply gravity
    velocity += world->gravity;
    if (velocity.y > terminalVelocity)
        velocity.y = terminalVelocity;

    // apply wind
    velocity += world->wind;

    // enforce max velocity
    float maxSpeed = 10.f;
    if (util::len(velocity) > maxSpeed) {
        velocity = (velocity / util::len(velocity)) * maxSpeed;
    }

    if (!stuck)
        position += velocity; // move due to forces

    // loop throuh all entities
    EntityTerrain *terrain = NULL;
    for (Entity *e : entities) {
        if (e->get_tag() == "terrain") { // get terrain entity
            terrain = (EntityTerrain*)e;
        }
        if (e->get_tag() == "dude") {
            if (stuckToEntity && e == stuckTo) {
                position = e->position + stuckOffset;
            }
            if (intersects(*e) && !stuck) { // react to collision with dude (bounce off)
                position -= velocity;
                sf::Vector2f normal = position - e->position;
                sf::Vector2f normalUnit;
                if (util::len(normal) == 0.f)
                    normalUnit = sf::Vector2f(0.f, -1.f);
                else
                    normalUnit = normal / util::len(normal);
                //velocity += normalUnit * (util::len(velocity) + bounciness);
                velocity += normalUnit * util::len(velocity); // set grenade direction to bounce off dude
                position += normalUnit * util::len(velocity); // move towards bouncing velocity
                collided = true;
                if (sticky) {
                    stuck = true;
                    stuckToEntity = true;
                }
                stuckTo = e;
                stuckOffset = position - e->position;
            }
        }
    }

    sf::Vector2f contact;
    if (terrain->intersects_with_circle(position, collisionRadius, &contact, 16)) {
        collided = true;
        contact -= velocity;
        position -= velocity * 1.1f;
        if (sticky)
            stuck = true;
        if (!rest) {
            float speed = util::distance(0.f, 0.f, velocity.x, velocity.y);
            float bounciness = speed * 0.2f; // model momentum
            sf::Vector2f normal = terrain->get_normal(contact);
            sf::Vector2f contactForce = normal * (speed + bounciness);
            velocity += contactForce;
            position += normal * 2.f; // move towards bouncing velocity
        }
    }

    if (position == oldPos) {
        if (restTimer.getElapsedTime().asSeconds() > 1.f)
            rest = true;
    } else {
        rest = false;
        restTimer.restart();
    }

    if (!collided)
        explodeTimer.restart();
    else if (explodeTimer.getElapsedTime().asSeconds() > 3.f ) { // time to explode
        remove = true;
    }

    if (remove) { // explode!
        if (cluster) {
            bool rip = false;
            world->add_entity(new EntityGrenade(position + sf::Vector2f(0.f, -32.f), sf::Vector2f(-2.f, -4.f), false, rip));
            world->add_entity(new EntityGrenade(position + sf::Vector2f(0.f, -32.f), sf::Vector2f(0.f, -4.f), false, rip));
            world->add_entity(new EntityGrenade(position + sf::Vector2f(0.f, -32.f), sf::Vector2f(2.f, -4.f), false, rip));
        }
        world->add_entity(new EntityExplosion(position));
        notify(EVENT_GRENADE_EXPLODE, (void*)this);
    }
    
}

sf::Texture& EntityGrenade::get_texture() {
    if (sticky) return txtSticky;
    if (cluster) return txtCluster;
    return txt;
}

void EntityGrenade::draw(sf::RenderWindow &window) {
    spr.setTexture(get_texture());
    spr.setOrigin(sf::Vector2f(collisionRadius, collisionRadius));
    spr.setPosition(position);
    spr.setRotation(angle);
    window.draw(spr);
    if (!stuck)
        angle += velocity.x * 3.f;
}