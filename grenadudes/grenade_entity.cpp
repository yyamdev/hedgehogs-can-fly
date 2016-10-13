#include "grenade_entity.h"
#include "terrain_entity.h"
#include "util.h"
#include <vector>
#include <tuple>
#include "build_options.h"
#include "explosion_entity.h"
#include "world.h"

sf::Vector2f EntityGrenade::gravity;
float EntityGrenade::terminalVelocity;

sf::Texture EntityGrenade::txt;
bool EntityGrenade::textureLoaded = false;

EntityGrenade::EntityGrenade() {
    EntityGrenade(sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 0.f), false);
}

EntityGrenade::EntityGrenade(sf::Vector2f pos, sf::Vector2f vel, bool sticky) {
    this->position = pos;
    this->velocity = vel;
    this->sticky = sticky;
    stuck = false;
    stuckToEntity = false;
    gravity = sf::Vector2f(0.f, GRAVITY);
    terminalVelocity = 6.f;
    tag = "grenade";
    collisionRadius = 8.f;
    rest = false;
    explodeTimer.restart();
    collided = false;
    bounciness = 2.f;
    invulnerable = false;
    hp = maxHp = START_HP_GRENADE;
    if (!textureLoaded) {
        txt.loadFromFile("data/grenade.png");
        textureLoaded = true;
    }
    angle = 0.f;
}

void EntityGrenade::event(sf::Event &e) {
}

void EntityGrenade::tick(std::vector<Entity*> &entities) {
    sf::Vector2f oldPos = position;
    // apply gravity
    velocity += gravity;
    if (velocity.y > terminalVelocity)
        velocity.y = terminalVelocity;

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
                sf::Vector2f normalUnit = normal / util::len(normal);
                velocity += normalUnit * (util::len(velocity) + bounciness);
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

    // detect collision with terrain
    std::vector< std::tuple<sf::Vector2f, bool> > probes;
    probes.push_back(std::tuple<sf::Vector2f, bool>(sf::Vector2f(position.x + collisionRadius, position.y), false));
    probes.push_back(std::tuple<sf::Vector2f, bool>(sf::Vector2f(position.x - collisionRadius, position.y), false));
    probes.push_back(std::tuple<sf::Vector2f, bool>(sf::Vector2f(position.x, position.y + collisionRadius), false));
    probes.push_back(std::tuple<sf::Vector2f, bool>(sf::Vector2f(position.x, position.y - collisionRadius), false));
    for (auto &probe : probes) {
        if (terrain->get_solid(std::get<0>(probe))) {
            std::get<1>(probe) = true;
            break;
        }
    }

    // react to collision with terrain
    /*
        todo -> possible improvement
        currently, collision is detected and contact point recorded in world space
        then entity is moved back to before collision, then normal is calculated at contact point.

        if contact point is too far into terrain, normals won't be calculated correctly.

        improvement: store contact point relative to the entity, then move the entity back to before collision.
        then calculate normal based on the contact point now transformed into world space.
        this way the contact point should mostly be next to the terrain edge more of the time.
        (unless entity is traveling way too fast, then neither of these methods will work)
    */
    for (auto &probe : probes) {
        if (std::get<1>(probe) && !stuck) { // collided at this probe point?
            collided = true;
            sf::Vector2f contact = std::get<0>(probe);
            position -= velocity * 1.1f;
            if (sticky)
                stuck = true;
            if (!rest) {
                float speed = util::distance(0.f, 0.f, velocity.x, velocity.y);
                sf::Vector2f normal = terrain->get_normal(contact);
                sf::Vector2f contactForce = normal * (speed + bounciness);
                velocity += contactForce;
            }
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
        world->add_entity(new EntityExplosion(position));
        notify(EVENT_GRENADE_EXPLODE, (void*)this);
    }
    
}

void EntityGrenade::draw(sf::RenderWindow &window) {
    spr.setTexture(txt);
    spr.setOrigin(sf::Vector2f(collisionRadius, collisionRadius));
    spr.setPosition(position);
    spr.setRotation(angle);
    window.draw(spr);
    if (!stuck)
        angle += util::sign(velocity.x) * 3.f;
}