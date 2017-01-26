#include "ball_entity.h"
#include "terrain_entity.h"
#include "util.h"
#include "build_options.h"
#include "world.h"
#include <iostream>
#include "util.h"

sf::Texture EntityBall::txt;
sf::Texture EntityBall::txtPoint;
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
        txtPoint.loadFromFile("data/point.png");
        textureLoaded = true;
    }
    terrain = NULL;
    dragging = false;
    disable = false;
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
    window.draw(spr);

    sprPoint.setTexture(txtPoint);
    sprPoint.setOrigin(sf::Vector2f(collisionRadius, collisionRadius));
    sprPoint.setPosition(contactPoint);
    window.draw(sprPoint);
}

void EntityBall::tick(std::vector<Entity*> &entities) {
    if (disable) return;
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

    if (terrain) {
        sf::Vector2f contact;
        if (terrain->intersects_with_circle(position, velocity, collisionRadius, &contact)) { // collision with terrain
            //disable = true;
            contactPoint = contact;
            
            float impactSpeed = util::len(velocity);
            if (impactSpeed < 0.62f) impactSpeed = 0.f;
            sf::Vector2f impactDirection = util::normalize(velocity);
            //contact -= velocity; // move contact point to terrain edge
            position -= velocity * 1.1f; // move out of collision
            // calculate vectors
            sf::Vector2f normal = util::normalize(terrain->get_normal(contact));
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

            //velocity = bounce + (slideDirection * slideAmmount);
            velocity = bounce;
            
            
        }
    }
}