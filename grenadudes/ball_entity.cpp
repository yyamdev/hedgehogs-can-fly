#include "ball_entity.h"
#include "terrain_entity.h"
#include "util.h"
#include "build_options.h"
#include "world.h"
#include <iostream>
#include "util.h"
#include "debug_draw.h"
#include "tnt_entity.h"

sf::Texture EntityBall::txt;
sf::Texture EntityBall::txtPoint;
bool EntityBall::textureLoaded = false;

EntityBall::EntityBall() {
    EntityBall(sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 0.f));
}

EntityBall::EntityBall(sf::Vector2f pos, sf::Vector2f vel){
    position = prevRest = pos;
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
    rest = false;
}

void EntityBall::event(sf::Event &e) {
    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Space) {
        rest = true;
        position = prevRest;
        notify(EVENT_BALL_REST_POS, (void*)(&position));
    }
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        if (!dragging && rest) {
            dragging = true;
            dragStart.x = (float)e.mouseButton.x;
            dragStart.y = (float)e.mouseButton.y;
            notify(EVENT_PLAYER_START_DRAG, (void*)(&dragStart));
        }
    }
    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        if (dragging) {
            dragging = false;
            notify(EVENT_PLAYER_END_DRAG, NULL);

            sf::Vector2f mouse;
            mouse.x = (float)e.mouseButton.x;
            mouse.y = (float)e.mouseButton.y;
            sf::Vector2f dir = dragStart - mouse;
            if (util::len(dir) != 0.f) {
                float speed = util::len(dragStart - mouse) / 15.f;

                speed = util::clamp(speed, 0.f, BALL_MAX_LAUNCH_SPEED - 2.f);
                sf::Vector2f start = position;
                dir = dir / util::len(dir) * speed;

                velocity = dir;
                rest = false;
                clkRest.restart();
                notify(EVENT_BALL_START_MOVING, NULL);
            }
        }
    }
}

void EntityBall::draw(sf::RenderWindow &window) {
    spr.setTexture(txt);
    spr.setOrigin(sf::Vector2f(collisionRadius, collisionRadius));
    spr.setPosition(position - world->camera);
    window.draw(spr);
}

void EntityBall::tick(std::vector<Entity*> &entities) {
    // centre camera
    if (!world->is_paused()) {
        sf::Vector2f screenSize((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
        sf::Vector2f delta = (position - screenSize / 2.f) - world->camera;
        world->camera += delta * 0.05f;
    }

    if (rest) return;
        
    // move
    sf::Vector2f oldPos = position;
    position += velocity;

    if (util::len(oldPos - position) > 1.5f) {
        clkRest.restart();
        rest = false;
        notify(EVENT_BALL_START_MOVING, NULL);
    }
    if (clkRest.getElapsedTime().asSeconds() > 1.f) {
        rest = true;
        prevRest = position;
        notify(EVENT_BALL_REST_POS, (void*)(&position));
    }

    // apply gravity
    velocity += world->gravity;

    // cap y speed
    if (velocity.y > BALL_TERM_VEL)
        velocity.y = BALL_TERM_VEL;

    // cap speed
    float speed = util::len(velocity);
    if (speed > BALL_MAX_SPEED) {
        velocity = util::normalize(velocity) * BALL_MAX_SPEED;
    }

    // apply air resistance
    velocity *= .995f;

    // find terrain
    for (Entity *e : entities) {
        if (!terrain && e->get_tag() == "terrain") { // get terrain entity
            terrain = (EntityTerrain*)e;
            break;
        }
        if (e->get_tag() == "ball" && e != this && intersects(*e)) {
            float impactSpeed = util::len(velocity);
            impactSpeed += 0.1f;
            sf::Vector2f impactDirection = util::normalize(velocity);
            position -= velocity * 1.1f;
            sf::Vector2f normal = util::normalize(position - e->position);
            sf::Vector2f reflect = impactDirection - 2.f * normal * (util::dot(impactDirection, normal));
            velocity = reflect * impactSpeed * .6f;
        }
        if (e->get_tag() == "tnt" && ((EntityTnt*)e)->intersects_with_circle(position, collisionRadius)) {
            EntityTnt *tnt = (EntityTnt*)e;
            tnt->touch();
            position -= velocity * 1.5f;
            bounce(.6f, tnt->get_normal(position, velocity));
        }
    }

    if (terrain) {
        sf::Vector2f contact = sf::Vector2f(50.f, 50.f);
        if (terrain->intersects_with_circle(position, velocity, collisionRadius, &contact, &position)) { // collision with terrain
            contactPoint = contact;

            TerrainType t = terrain->get_pos(contactPoint);

            float bounceFactor = .6f;
            if (t == T_BOUNCY) bounceFactor = 1.2f;
            if (t == T_SLOW) bounceFactor = 0.3f;
            if (t == T_STICKY) {
                rest = true;
                prevRest = position;
                bounceFactor = 0.0f;
                notify(EVENT_BALL_REST_POS, (void*)(&position));
            }

            bounce(bounceFactor, terrain->get_normal(contact));
        }
    }
}

void EntityBall::bounce(float bounceFactor, sf::Vector2f norm) {
    float impactSpeed = util::len(velocity);
    sf::Vector2f impactDirection;
    if (impactSpeed != 0.f)
        impactDirection = util::normalize(velocity);

    // calculate vectors
    sf::Vector2f normal = util::normalize(norm);
    sf::Vector2f reflect = velocity - 2.f * normal * (util::dot(velocity, normal));
    sf::Vector2f bounce = sf::Vector2f();

    if (impactSpeed > 0.2f && impactSpeed < 3.1f) { // bounce a little
        bounce = reflect * bounceFactor;
    }
    else if (impactSpeed >= 3.1f) { // bounce a lot
        bounce = reflect * bounceFactor;
    }

    if (util::dot(normal, util::normalize(reflect)) < 0.3f) {
        bounce += normal;
    }

    velocity = bounce;
}

void EntityBall::on_notify(Event event, void *data) {
    if (event == EVENT_TERRAIN_CHANGE) {
        rest = false;
        clkRest.restart();
    }
}

bool EntityBall::is_at_rest() {
    return rest;
}