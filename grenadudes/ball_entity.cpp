#include "ball_entity.h"
#include "terrain_entity.h"
#include "util.h"
#include "build_options.h"
#include "world.h"
#include <iostream>
#include "util.h"

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
    dragging = false;
}

void EntityBall::event(sf::Event &e) {
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        if (!dragging) {
            dragging = true;
            dragStart.x = (float)e.mouseButton.x;
            dragStart.y = (float)e.mouseButton.y;
            notify(EVENT_PLAYER_START_DRAG, (void*)(&dragStart));
        }
    }
    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        if (dragging) {
            dragging = false;
            sf::Vector2f mouse;
            mouse.x = (float)e.mouseButton.x;
            mouse.y = (float)e.mouseButton.y;
            sf::Vector2f dir = dragStart - mouse;
            float speed = util::len(dragStart - mouse) / 15.f;

            speed = util::clamp(speed, 0.f, 10.f);
            sf::Vector2f start = position;
            dir = dir / util::len(dir) * speed;

            //world->add_entity(new EntityBall(start, dir));
            velocity = dir;
            //dude->throw_grenade(dragStart - mouse, util::len(dragStart - mouse) / 15.f);
            notify(EVENT_PLAYER_END_DRAG, NULL);
        }
    }
}

void EntityBall::draw(sf::RenderWindow &window) {
    spr.setTexture(txt);
    spr.setOrigin(sf::Vector2f(collisionRadius, collisionRadius));
    spr.setPosition(position);
    spr.setRotation(spr.getRotation() + velocityAngular);
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
    if (velocity.y > BALL_TERM_VEL)
        velocity.y = BALL_TERM_VEL;

    // find terrain
    for (Entity *e : entities) {
        if (!terrain && e->get_tag() == "terrain") { // get terrain entity
            terrain = (EntityTerrain*)e;
            break;
        }
        if (e->get_tag() == "ball" && e != this && intersects(*e)) {
            float impactSpeed = util::len(velocity);
            impactSpeed += 0.1f;
            //if (impactSpeed < 0.62f) impactSpeed = 0.f;
            sf::Vector2f impactDirection = util::normalize(velocity);
            position -= velocity * 1.1f;
            sf::Vector2f normal = util::normalize(position - e->position);
            sf::Vector2f reflect = impactDirection - 2.f * normal * (util::dot(impactDirection, normal));
            velocity = reflect * impactSpeed * .6f;
        }
    }

    // dampen angular velocity
    //velocityAngular *= .999f;
    //velocityAngular = velocity.x;

    if (terrain) {
        sf::Vector2f contact;
        if (terrain->intersects_with_circle(position, collisionRadius, &contact, 16)) { // collision with terrain
            float impactSpeed = util::len(velocity);
            if (impactSpeed < 0.62f) impactSpeed = 0.f;
            sf::Vector2f impactDirection = util::normalize(velocity);
            contact -= velocity; // move contact point to terrain edge
            position -= velocity * 1.1f; // move out of collision
            // calculate vectors
            sf::Vector2f normal = util::normalize(terrain->get_normal(contact));
            sf::Vector2f slideDirection = get_slide_down(velocity, normal, world->gravity);
            sf::Vector2f reflect = impactDirection - 2.f * normal * (util::dot(impactDirection, normal));
            sf::Vector2f bounce = sf::Vector2f();
            if (impactSpeed > 0.2f && impactSpeed < 3.1f) { // bounce a little
                bounce = reflect * impactSpeed * .6f;
                //std::cout << impactSpeed << " small\n";
            }
            else if (impactSpeed >= 3.1f) { // bounce a lot
                bounce = reflect * impactSpeed * .6f;
                //std::cout << impactSpeed << " big\n";
            }

            if (util::len(bounce) < .5f)
                bounce = sf::Vector2f();

            float slideAmmount = util::dot(util::normalize(velocity), slideDirection);
            if (slideAmmount < .4f) { // friction
                slideAmmount = 0.f;
            }

            //velocity = bounce + (slideDirection * slideAmmount);
            velocity = bounce;
            
        }
    }
}