#include <assert.h>
#include <iostream>
#include "ball_entity.h"
#include "terrain_entity.h"
#include "fireworks_entity.h"
#include "build_options.h"
#include "world.h"
#include "util.h"
#include "debug_draw.h"
#include "imgui.h"
#include "particle.h"
#include "options.h"
#include "state.h"

// Static members
sf::Texture EntityBall::txt;
sf::Texture EntityBall::txtPoint;
bool EntityBall::textureLoaded = false;

const float BALL_TERM_VEL = 9.f;
const float BALL_MAX_LAUNCH_SPEED = 12.f;
const float BALL_MAX_LAUNCH_SPEED_NERF = 6.f;
const float BALL_MAX_SPEED = 14.f;

EntityBall::EntityBall()
{
    EntityBall(sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 0.f), sf::Color::White);
}

EntityBall::~EntityBall()
{
    notify(EVENT_HUD_SET_ACTIVE_BALL, NULL);
}

EntityBall::EntityBall(sf::Vector2f pos, sf::Vector2f vel, sf::Color colour)
{
    if (!textureLoaded) {
        txt.loadFromFile("data/ball.png");
        txtPoint.loadFromFile("data/point.png");
        textureLoaded = true;
    }

    collisionRadius = 10.f;
    tag = "ball";
    velocity = vel;
    this->colour = colour;
    position = prevRest = pos;

    notify(EVENT_HUD_SET_ACTIVE_BALL, this);
}

void EntityBall::on_add()
{
    // Centre camera over this entity
    sf::Vector2f screenSize((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
    world->camera = (position - screenSize / 2.f);
}

void EntityBall::event(sf::Event &e)
{
    if (!reactToInput || State::get_current()->get_name() != "play")
        return;

    if (e.type == sf::Event::MouseMoved) {
        mouse.x = (float)e.mouseMove.x;
        mouse.y = (float)e.mouseMove.y;
    }

    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        if (!dragging && (canFling)) {
            dragging = true;
            dragStart.x = (float)e.mouseButton.x;
            dragStart.y = (float)e.mouseButton.y;
            notify(EVENT_START_DRAG, (void*)(&dragStart));
        }
    }

    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        if (dragging) {
            dragging = false;
            notify(EVENT_END_DRAG, NULL);
            mouse.x = (float)e.mouseButton.x;
            mouse.y = (float)e.mouseButton.y;
            sf::Vector2f dir = mouse - dragStart;
            if (util::len(dir) != 0.f) {
                float speed = 0.f;
                if (lastTerrain == T_SLOW) {
                    speed = util::len(mouse - dragStart) / 25.f;
                    speed = util::clamp(speed, 0.f, BALL_MAX_LAUNCH_SPEED_NERF);
                } else {
                    speed = util::len(mouse - dragStart) / 15.f;
                    speed = util::clamp(speed, 0.f, BALL_MAX_LAUNCH_SPEED);
                }
                sf::Vector2f start = position;
                dir = dir / util::len(dir) * speed;

                if (canFling || canNudge) {
                    if (canFling) {
                        record_new_rest_pos();
                        velocity = dir;
                        stop_resting();
                    }
                }
            }
            terrainFlungOn = lastTerrain;
            prevAngle = angle;
            notify(EVENT_BALL_FLING, NULL);
        }
    }
}

void EntityBall::draw(sf::RenderWindow &window)
{
    if (!world->is_paused())
        angle += velocity.x * 2.f;

    if (!dead) {
        spr.setTexture(txt);
        spr.setOrigin(sf::Vector2f(collisionRadius, collisionRadius));
        spr.setPosition(position - world->camera);
        spr.setRotation(angle);

        if (rest && dragging)
            spr.setScale((int)dragStart.x < sf::Mouse::getPosition(window).x ? 1.f : -1.f, 1.f);

        window.draw(spr);
    }

    if (util::len(velocity) > 0.f && (bool)options.trail && !world->is_paused())
        add_particle(position, sf::Vector2f(), sf::Vector2f(0.f, 0.f), colour, 120);

    if (edit && ImGui::CollapsingHeader("Ball")) {
        ImGui::Checkbox("react to input", &reactToInput);
        ImGui::Checkbox("move camera", &moveCamera);
    }
}

void EntityBall::tick(std::vector<Entity*> &entities)
{
    // Move camera toward this entity's position
    if (!world->is_paused() && moveCamera) {
        sf::Vector2f screenSize((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
        sf::Vector2f delta = (position - screenSize / 2.f) - world->camera;
        world->camera += delta * 0.05f;
    }

    if (rest)
        return;

    // Move to new position
    sf::Vector2f oldPos = position;
    position += velocity;

    // Nudge
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && canNudge && !canFling) {
        sf::Vector2f dir = util::normalize(mouse - (position - world->camera));
        velocity += dir * nudgeStr;
    }

    // Start or stop resting
    if (util::len(oldPos - position) > 1.5f)
        stop_resting();

    if (clkRest.getElapsedTime().asSeconds() > 1.f && touching_wall()) {
        record_new_rest_pos();
        rest = true;
    }

    // Update canFling
    if (util::len(velocity) < maxFlingVelocity && touching_wall() && !canFling)
        canFling = true;
    else if (util::len(velocity) > 2.f * maxFlingVelocity && !touching_wall() && canFling) // NOTE: why x2?
        canFling = false;
    notify(EVENT_BALL_CHANGE_CAN_FLING, (void*)(&canFling));

    // Update canNudge
    // TODO: Remove because this is never false.
    assert(canNudge == true);
    canNudge = true;
    notify(EVENT_BALL_CHANGE_CAN_NUDGE, (void*)(&canNudge));

    // Apply gravity
    velocity += world->gravity;

    // Cap falling speed
    if (velocity.y > BALL_TERM_VEL)
        velocity.y = BALL_TERM_VEL;

    // Cap velocity
    if (util::len(velocity) > BALL_MAX_SPEED) {
        velocity = util::normalize(velocity) * BALL_MAX_SPEED;
    }

    // Apply air resistance
    velocity *= .995f;

    // Get pointer to terrain entity
    if (!terrain) {
        for (Entity *e : entities) {
            if (e->get_tag() == "terrain") {
                terrain = (EntityTerrain*)e;
                break;
            }
        }
    }

    if (!terrain) // Nothing left to do if there is no terrain
        return;

    sf::Vector2f contact;
    if (terrain->intersects_with_circle(position, collisionRadius, &contact)) {
        // Record contact position
        lastContact = contact;

        // Move out of intersection
        position = contact + terrain->get_normal(contact) * collisionRadius;

        TerrainType t = terrain->get_pos(contact - terrain->get_normal(contact));

        // Handle special terrain types
        if (t == T_KILL) {
            for (int p = 0; p < 25; ++p) {
                add_particle(position,
                             sf::Vector2f(0.f, -3.f) + sf::Vector2f(util::rnd(-1.f, 1.f), util::rnd(-1.f, 1.f)),
                             sf::Vector2f(0.f, 0.1f),
                             sf::Color(127, 87, 99),
                             120);
            }
            reset_to_rest();
            notify(EVENT_BALL_HIT_WATER, NULL);
            return;
        }

        if (t == T_WIN) {
            if (!spawned_fireworks) {
                reactToInput = false;
                world->add_entity(new EntityFireworks(position));
                spawned_fireworks = true;
                notify(EVENT_LEVEL_COMPLETE, NULL);
            }
            return;
        }

        // Handle regular terrain types
        clkWallTouch.restart();
        lastTerrain = t;
        float bounceFactor = .6f;
        sf::Color particleColour = colour;

        bool spawn_particles = false;

        switch (t) {
        case T_BOUNCY:
            spawn_particles = true;
            particleColour = sf::Color(153, 229, 80);
            bounceFactor = 1.6f;
            notify(EVENT_BALL_HIT_BOUNCY, NULL);
            break;

        case T_SLOW:
            spawn_particles = false;
            bounceFactor = .3f;
            if (util::len(velocity) > .75f)
                notify(EVENT_BALL_HIT_SANDY, NULL);
            break;

        case T_STICKY:
            spawn_particles = true;
            particleColour = sf::Color(215, 123, 186);
            bounceFactor = 0.f;
            rest = true;
            record_new_rest_pos();
            notify(EVENT_BALL_HIT_STICKY, NULL);
            break;

        default:
            notify(EVENT_BALL_HIT_SOLID, NULL);
            break;
        }

        if (spawn_particles) {
            for (int p = 0; p < 10; ++p) {
                add_particle(position,
                             sf::Vector2f(0.f, -3.f) + sf::Vector2f(util::rnd(-1.f, 1.f), util::rnd(-1.f, 1.f)),
                             sf::Vector2f(0.f, 0.1f),
                             particleColour,
                             120);
            }
        }

        bounce(bounceFactor, terrain->get_normal(contact));
    }

}

void EntityBall::bounce(float bounceFactor, sf::Vector2f norm)
{
    float impactSpeed = util::len(velocity);
    sf::Vector2f impactDirection;
    if (impactSpeed != 0.f)
        impactDirection = util::normalize(velocity);

    sf::Vector2f normal = util::normalize(norm);
    sf::Vector2f reflect = velocity - 2.f * normal * (util::dot(velocity, normal));
    sf::Vector2f bounce;

    if (impactSpeed > 0.2f)
        bounce = reflect * bounceFactor;

    if (util::dot(normal, util::normalize(reflect)) < 0.3f)
        bounce += normal;

    velocity = bounce;
}

void EntityBall::record_new_rest_pos()
{
    // Called when ball has started resting
    prevRest = position;
    canFling = true;
    notify(EVENT_BALL_CHANGE_CAN_FLING, &canFling);

    velocity.x = 0.f;
    velocity.y = 0.f;

    if (!terrain)
        angle = 0.f;
    else {
        sf::Vector2f normal = terrain->get_normal(lastContact);
        angle = atan2(normal.x, -normal.y) * (180.f / 3.14f);
    }
    prevAngle = angle;
}

void EntityBall::reset_to_rest()
{
    rest = true;

    position = prevRest;
    angle = prevAngle;
    velocity = sf::Vector2f();

    canFling = true;
    notify(EVENT_BALL_CHANGE_CAN_FLING, &canFling);

    lastTerrain = terrainFlungOn;
}

void EntityBall::stop_resting()
{
    rest = false;
    clkRest.restart();
}

bool EntityBall::is_at_rest()
{
    return rest;
}

bool EntityBall::is_on_sand()
{
    return lastTerrain == T_SLOW;
}

bool EntityBall::touching_wall()
{
    return clkWallTouch.getElapsedTime().asMilliseconds() < 100;
}

bool EntityBall::can_fling()
{
    return canFling;
}

bool EntityBall::can_nudge()
{
    return canNudge;
}
