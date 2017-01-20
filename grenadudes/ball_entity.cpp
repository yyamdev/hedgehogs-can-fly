#include "ball_entity.h"
#include "terrain_entity.h"
#include "util.h"
#include "build_options.h"
#include "world.h"
#include <iostream>

sf::Texture EntityBall::txt;
bool EntityBall::textureLoaded = false;

EntityBall::EntityBall() {
    EntityBall(sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 0.f));
}

EntityBall::EntityBall(sf::Vector2f pos, sf::Vector2f vel){
    position = pos;
    velocity = vel;
    tag = "ball";
    collisionRadius = 8.f;
    invulnerable = true;
    hp = maxHp = 10;
    if (!textureLoaded) {
        txt.loadFromFile("data/ball.png");
        textureLoaded = true;
    }
    terrain = NULL;
    velocityAngular = 0.f;
}

void EntityBall::event(sf::Event &e) {
}

void EntityBall::draw(sf::RenderWindow &window) {
    spr.setTexture(txt);
    spr.setOrigin(sf::Vector2f(collisionRadius, collisionRadius));
    spr.setPosition(position);
    window.draw(spr);
}

sf::Vector2f EntityBall::get_slide_down(sf::Vector2f incidence, sf::Vector2f normal, sf::Vector2f gravity) {
    incidence = util::normalize(incidence);
    normal = util::normalize(normal);
    gravity = util::normalize(gravity);
    // convert to homogenous
    sf::Vector3f incidenceH = sf::Vector3f(incidence.x, incidence.y, 1.f);
    sf::Vector3f normalH = sf::Vector3f(normal.x, normal.y, 1.f);
    sf::Vector3f gravityH = sf::Vector3f(gravity.x, gravity.y, 1.f);
    // get slide up or down
    sf::Vector3f outH = sf::Vector3f(0.f, 0.f, -1.f);
    sf::Vector3f slideH = util::cross(outH, normalH);
    // reverse slide direction if needed
    if (util::dot3(slideH, gravityH) < 0.f)
        slideH = -slideH;
    else if (util::dot3(slideH, gravityH) == 0.f) {
        if (util::dot3(slideH, incidenceH) < 0.f)
            slideH = -slideH;
    }
    // convert to regular 2d
    return sf::Vector2f(slideH.x, slideH.y);
}

void EntityBall::tick(std::vector<Entity*> &entities) {
    sf::Vector2f oldPos = position;
    position += velocity;

    // apply gravity
    velocity += world->gravity;

    // cap y speed
    if (fabs(velocity.y) > BALL_TERM_VEL)
        velocity.y = BALL_TERM_VEL * util::sign(velocity.y);

    // find terrain
    if (!terrain) {
        for (Entity *e : entities) {
            if (e->get_tag() == "terrain") { // get terrain entity
                terrain = (EntityTerrain*)e;
                break;
            }
        }
    }

    if (terrain) {
        sf::Vector2f contact;
        if (terrain->intersects_with_circle(position, collisionRadius, &contact, 16)) { // collision with terrain
            float impactSpeed = util::len(velocity);
            position -= velocity * 1.1f; // move out of collision
            contact -= velocity; // move contact point to terrain edge

            // calculate vectors
            sf::Vector2f normal = terrain->get_normal(contact);
            sf::Vector2f slide = get_slide_down(velocity, normal, world->gravity);
            sf::Vector2f bounce = sf::Vector2f();
            if (impactSpeed > 1.5f) // bounce
                bounce = util::normalize(velocity + (2.f * normal)) * util::len(velocity) * .8f; // vector reflection, lose some speed due to energy loss
            float slideAmmount = util::dot(util::normalize(world->gravity), slide);
            if (slideAmmount < .3f) slideAmmount = 0.f; // friction

            velocity = slide * slideAmmount;
        }
    }
}