#include "ball_entity.h"
#include "terrain_entity.h"
#include "util.h"
#include "build_options.h"
#include "world.h"
#include <iostream>
#include "util.h"
#include "debug_draw.h"
#include "tnt_entity.h"
#include "imgui.h"

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
    dragMode = DM_NUDGE;
    flingTimerValue = 1000;
    reactToInput = true;
    canFling = false;
    canNudge = false;
    nudgeStr = 0.05f;
    maxFlingVelocity = 1.4f;
}

void EntityBall::event(sf::Event &e) {
    if (!reactToInput) return;

    if (e.type == sf::Event::MouseMoved) {
        mouse.x = (float)e.mouseMove.x;
        mouse.y = (float)e.mouseMove.y;
    }
    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Space) {
        reset_to_rest();
        rest = true;
        record_new_rest_pos();
        notify(EVENT_PRESS_SPACE, NULL);
    }
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        if (!dragging && (canFling)) {
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

                if (canFling || canNudge) {
                    switch (dragMode) {
                    case DM_REST:
                        record_new_rest_pos();
                        velocity = dir;
                        stop_resting();
                        break;
                    case DM_TIME:
                        clkFlingTimer.restart();
                        velocity = dir;
                        stop_resting();
                        break;
                    case DM_NUDGE:
                        if (canFling) {
                            record_new_rest_pos();
                            velocity = dir;
                            stop_resting();
                        }
                        break;
                    }
                }
            }
        }
    }
}

void EntityBall::draw(sf::RenderWindow &window) {
    spr.setTexture(txt);
    spr.setOrigin(sf::Vector2f(collisionRadius, collisionRadius));
    spr.setPosition(position - world->camera);
    window.draw(spr);

    if (edit && ImGui::CollapsingHeader("Ball")) {
        ImGui::Checkbox("react to input", &reactToInput);
        ImGui::Separator();
        ImGui::Text("wall clock: %d", clkWallTouch.getElapsedTime().asMilliseconds());
        ImGui::Text("speed: %f", util::len(velocity));
        ImGui::Separator();
        sf::Color dragCol = sf::Color::White;
        ImGui::TextColored(dragCol, "Drag Mode");
        ImGui::RadioButton("rest", (int*)(&dragMode), 0); ImGui::SameLine();
        ImGui::RadioButton("timer", (int*)(&dragMode), 1); ImGui::SameLine();
        ImGui::RadioButton("nudge", (int*)(&dragMode), 2);
        ImGui::DragFloat("max fling velocity", &maxFlingVelocity, 0.01f, 0.f, 4.f);
        if (dragMode == DM_TIME) {
            ImGui::DragInt("fling timer threshold (ms)", &flingTimerValue, 0, 10000);
        }
        if (dragMode == DM_NUDGE) {
            ImGui::DragFloat("nudge strength", &nudgeStr, 0.01f, 0.f, 1.f);
        }
        ImGui::Separator();
    }
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
    notify(EVENT_BALL_MOVE, (void*)(&position));

    // handle nudging
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && dragMode == DM_NUDGE && canNudge) {
        sf::Vector2f dir = util::normalize(mouse - (position - world->camera));
        velocity += dir * nudgeStr;
    }

    if (util::len(oldPos - position) > 1.5f) {
        stop_resting();
    }
    if (clkRest.getElapsedTime().asSeconds() > 1.f && touching_wall()) {
        record_new_rest_pos();
        rest = true;
    }

    // update canFling
    if (dragMode == DM_REST || dragMode == DM_NUDGE) {
        if (util::len(velocity) < maxFlingVelocity && rest)
            canFling = true;
        else
            canFling = false;
    }
    else if (dragMode == DM_TIME) {
        if (clkFlingTimer.getElapsedTime().asMilliseconds() > flingTimerValue)
            canFling = true;
        else
            canFling = false;
    }
    notify(EVENT_BALL_CHANGE_CAN_FLING, (void*)(&canFling));

    // update canNudge
    if (dragMode == DM_NUDGE) {
        if (1)
            canNudge = true;
        else
            canNudge = false;
    } else canNudge = false;
    notify(EVENT_BALL_CHANGE_CAN_NUDGE, (void*)(&canNudge));

    // apply gravity
    velocity += world->gravity;

    // cap y speed
    // TODO -> handle gravity being any direction
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
        // test if hit water
        if (terrain->get_pos(position) == T_KILL) {
            reset_to_rest();
            notify(EVENT_HIT_WATER, NULL);
        }

        sf::Vector2f contact = sf::Vector2f(50.f, 50.f);
        if (terrain->intersects_with_circle(position, velocity, collisionRadius, &contact, &position)) { // collision with terrain
            contactPoint = contact;

            TerrainType t = terrain->get_pos(contactPoint);

            clkWallTouch.restart();

            float bounceFactor = .6f;
            if (t == T_BOUNCY) bounceFactor = 1.2f;
            if (t == T_SLOW) bounceFactor = 0.3f;
            if (t == T_STICKY) {
                rest = true;
                record_new_rest_pos();
                bounceFactor = 0.0f;
            }
            if (t == T_THIN) {
                float impactSpeed = util::len(velocity);
                if (impactSpeed > 9.f) {
                    terrain->remove_flood_fill(contactPoint);
                    velocity = util::normalize(velocity) * fmax(0.f, impactSpeed - 7.f);
                    notify(EVENT_SMASH_DOOR, NULL);
                    return;
                } else {
                    notify(EVENT_BOUNCE_DOOR, NULL);
                }
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
    if (event == EVENT_TNT_EXPLODE) {
        sf::Vector3f *info = (sf::Vector3f*)data;
        sf::Vector2f centre(info->x, info->y);
        sf::Vector2f tntToBall = position - centre;
        float maxSpeed = 18.f;
        float speed = maxSpeed * (1.f -  fmin(1.f, util::len(tntToBall) / info->z));
        velocity += util::normalize(tntToBall) * speed;
    }
    if (event == EVENT_NEW_WORLD_GRAVITY) {
        stop_resting();
    }
}

bool EntityBall::is_at_rest() {
    return rest;
}

bool EntityBall::touching_wall() {
    return clkWallTouch.getElapsedTime().asMilliseconds() < 400;
}

void EntityBall::record_new_rest_pos() {
    // called when ball is at rest
    prevRest = position;
    notify(EVENT_BALL_REST_POS, (void*)(&position));
    if (dragMode == DM_REST || dragMode == DM_NUDGE) {
        canFling = true;
        notify(EVENT_BALL_CHANGE_CAN_FLING, &canFling);
    }
}

void EntityBall::stop_resting() {
    rest = false;
    clkRest.restart();
}

void EntityBall::reset_to_rest() {
    position = prevRest;
    velocity = sf::Vector2f();
}