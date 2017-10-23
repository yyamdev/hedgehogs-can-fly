#include "ball_entity.h"
#include "terrain_entity.h"
#include "util.h"
#include "build_options.h"
#include "world.h"
#include <iostream>
#include "util.h"
#include "debug_draw.h"
#include "imgui.h"
#include "particle.h"
#include "gate.h"
#include "options.h"
#include "enemy_entity.h"
#include "death_state.h"
#include "state.h"
#include "hud.h"
#include "fireworks_entity.h"

sf::Texture EntityBall::txt;
sf::Texture EntityBall::txtPoint;
bool EntityBall::textureLoaded = false;

const float BALL_TERM_VEL = 9.f;
const float BALL_MAX_LAUNCH_SPEED = 14.f;
const float BALL_MAX_LAUNCH_SPEED_NERF = 8.f;
const float BALL_MAX_SPEED = 14.f;

EntityBall::EntityBall() {
    EntityBall(sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 0.f), sf::Color::White, NULL);
    std::cout << "Don't call EntityBall deafult constructor!\n";
}

EntityBall::~EntityBall() {
    ballHud = NULL;
}

EntityBall::EntityBall(sf::Vector2f pos, sf::Vector2f vel, sf::Color colour, bool *restartFlag) {
    if (!textureLoaded) {
        txt.loadFromFile("data/ball.png");
        txtPoint.loadFromFile("data/point.png");
        textureLoaded = true;
    }

    ballHud = this;
    this->restartFlag = restartFlag;
    this->colour = colour;
    position = prevRest = pos;
    velocity = vel;
    tag = "ball";
    collisionRadius = 8.f;
    terrain = NULL;
    enemy = NULL;
    dead = false;
    dragging = false;
    rest = false;
    reactToInput = true;
    canFling = false;
    canNudge = false;
    nudgeStr = 0.04f;
    maxFlingVelocity = 1.2f;
    spawned_fireworks = false;
    justSpawned = true;
}

void EntityBall::event(sf::Event &e) {
    if (!reactToInput || State::get_current()->get_name() != "play") return;

    if (e.type == sf::Event::MouseMoved) {
        mouse.x = (float)e.mouseMove.x;
        mouse.y = (float)e.mouseMove.y;
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
            sf::Vector2f dir = mouse - dragStart;
            if (util::len(dir) != 0.f) {
                float speed = 0.f;
                if (lastTerrain == T_SLOW) {
                    speed = util::len(mouse - dragStart) / 25.f;
                    speed = util::clamp(speed, 0.f, BALL_MAX_LAUNCH_SPEED_NERF - 2.f);
                } else {
                    speed = util::len(mouse - dragStart) / 15.f;
                    speed = util::clamp(speed, 0.f, BALL_MAX_LAUNCH_SPEED - 2.f);
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
            notify(EVENT_BALL_FLING, NULL);
        }
    }
}

void EntityBall::draw(sf::RenderWindow &window) {
    static float angle = 0.f;
    angle += velocity.x * 2.f;
    if (!dead) {
        spr.setTexture(txt);
        spr.setOrigin(sf::Vector2f(collisionRadius, collisionRadius));
        spr.setPosition(position - world->camera);
        spr.setColor(colour);
        spr.setRotation(angle);
        window.draw(spr);
    }

    if (util::len(velocity) > 0.f && (bool)options.trail && !world->is_paused())
        add_particle(position, sf::Vector2f(), sf::Vector2f(0.f, 0.f), colour, 120);

    if (edit && ImGui::CollapsingHeader("Ball")) {
        ImGui::Checkbox("react to input", &reactToInput);
        ImGui::Separator();
        ImGui::Text("wall clock: %d", clkWallTouch.getElapsedTime().asMilliseconds());
        ImGui::Text("speed: %f", util::len(velocity));
    }
}

bool EntityBall::is_on_sand()
{
    return lastTerrain == T_SLOW;
}

void EntityBall::tick(std::vector<Entity*> &entities) {
    // centre camera
    if (justSpawned) {
        sf::Vector2f screenSize((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
        world->camera = (position - screenSize / 2.f);
        justSpawned = false;
    }
    // move camera
    if (!world->is_paused() && !world->cameraFollowBall) {
        sf::Vector2f screenSize((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
        sf::Vector2f delta = (position - screenSize / 2.f) - world->camera;
        world->camera += delta * 0.05f;
    }

    // Handle enemy collision even if at rest
    if (enemy && enemy->is_active()) {
        if (util::len(position - enemy->position) < collisionRadius + enemy->collisionRadius &&
            !dead) {
            dead = true;
            reactToInput = false;
            rest = true;
            deadTimer.restart();
            for (int i = 0; i < 80; ++i) {
                add_particle(position,
                             sf::Vector2f(util::rnd(-4.f, 4.f), util::rnd(-4.f, 4.f)),
                             sf::Vector2f(0.f, .1f),
                             colour,
                             120);
            }
        }
    }

    if (dead && deadTimer.getElapsedTime().asSeconds() > 2.f) {
        std::cout << "PUSH DEATH STATE\n";
        State::push_state(new StateDeath(world, restartFlag, State::get_current()->get_clear_colour()));
    }

    if (rest) return;
        
    // move
    sf::Vector2f oldPos = position;
    position += velocity;
    notify(EVENT_BALL_MOVE, (void*)(&position));

    // handle nudging
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && canNudge && !canFling) {
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
    if (util::len(velocity) < maxFlingVelocity && touching_wall() && !canFling)
        canFling = true;
    else if (util::len(velocity) > 2.f * maxFlingVelocity && !touching_wall() && canFling)
        canFling = false;
    notify(EVENT_BALL_CHANGE_CAN_FLING, (void*)(&canFling));

    // update canNudge
    canNudge = true;
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

    for (Entity *e : entities) {
        if (!terrain && e->get_tag() == "terrain") { // get terrain entity
            terrain = (EntityTerrain*)e;
            break;
        }
        if (!enemy && e->get_tag() == "enemy") { // get enemy entity
            enemy = (EntityEnemy*)e;
            break;
        }
        if (e->get_tag() == "gate") {
            EntityGate *gate = (EntityGate*)e;
            if (gate->intersects_circle(position, collisionRadius)) {
                velocity += gate->get_boost_vector();
            }
        }
    }

    if (terrain) {
        // test if hit lava
        if (terrain->get_pos(position) == T_KILL) {
            for (int p = 0; p < 25; ++p) {
                add_particle(position,
                             sf::Vector2f(0.f, -3.f) + sf::Vector2f(util::rnd(-1.f, 1.f), util::rnd(-1.f, 1.f)),
                             sf::Vector2f(0.f, 0.1f),
                             sf::Color(127, 87, 99),
                             120);
            }
            reset_to_rest();
            notify(EVENT_BALL_HIT_WATER, NULL);
        }

        if (terrain->get_pos(position) == T_THIN) {
            // Hijaking this to spawn the enemy
            notify(EVENT_BALL_HIT_TRIGGER, NULL);
        }

        sf::Vector2f contact;
        if (terrain->intersects_with_circle(position, velocity, collisionRadius, &contact, &position)) { // collision with terrain
            contactPoint = contact;

            TerrainType t = terrain->get_pos(contactPoint);
            if (t == T_WIN) {
                if (!spawned_fireworks) {
                    reactToInput = false;
                    world->add_entity(new EntityFireworks(position));
                    spawned_fireworks = true;
                }
                //notify(EVENT_BALL_HIT_FINISH, NULL);
            }

            clkWallTouch.restart();

            float bounceFactor = .6f;
            sf::Color particleColour = colour;
            bool particles = false;
            if (t == T_BOUNCY) {
                bounceFactor = 1.6f;
                particleColour = sf::Color(153, 229, 80);
                particles = true;
                lastTerrain = T_BOUNCY;
                notify(EVENT_BALL_HIT_BOUNCY, NULL);
            }
            else if (t == T_SLOW) {
                particles = false;
                bounceFactor = 0.3f;
                lastTerrain = T_SLOW;
                if (util::len(velocity) > .75f)
                    notify(EVENT_BALL_HIT_SANDY, NULL);
            }
            else if (t == T_STICKY) {
                rest = true;
                record_new_rest_pos();
                bounceFactor = 0.0f;
                particleColour = sf::Color(215, 123, 186);
                particles = true;
                lastTerrain = T_STICKY;
                notify(EVENT_BALL_HIT_STICKY, NULL);
            }
            else if (t == T_THIN) {
                // Used for something else
                /*
                float impactSpeed = util::len(velocity);
                if (impactSpeed > 9.f) {
                    // TODO -> Increase updateRect or actualy calculate it using flood fill data? (Currently can be too small)
                    terrain->remove_flood_fill(contactPoint);
                    sf::Rect<unsigned int> updateRect((unsigned int)position.x - 96, (unsigned int)position.y - 96, 96 * 2, 96 * 2);
                    notify(EVENT_TERRAIN_CHANGE, &updateRect);
                    velocity = util::normalize(velocity) * fmax(0.f, impactSpeed - 7.f);
                    notify(EVENT_SMASH_DOOR, NULL);
                    return;
                } else {
                    notify(EVENT_BOUNCE_DOOR, NULL);
                }
                */
            }
            else {
                notify(EVENT_BALL_HIT_SOLID, NULL);
                lastTerrain = T_SOLID;
            }

            if (particles) {
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
}

void EntityBall::bounce(float bounceFactor, sf::Vector2f norm) {
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

void EntityBall::on_notify(Event event, void *data) {
    if (event == EVENT_TERRAIN_CHANGE) {
        rest = false;
        clkRest.restart();
    }
    if (event == EVENT_NEW_WORLD_GRAVITY) {
        stop_resting();
    }
}

bool EntityBall::is_at_rest() {
    return rest;
}

bool EntityBall::touching_wall() {
    return clkWallTouch.getElapsedTime().asMilliseconds() < 100;
}

void EntityBall::record_new_rest_pos() {
    // called when ball is at rest
    prevRest = position;
    notify(EVENT_BALL_REST_POS, (void*)(&position));
    canFling = true;
    notify(EVENT_BALL_CHANGE_CAN_FLING, &canFling);
}

void EntityBall::stop_resting() {
    rest = false;
    clkRest.restart();
}

void EntityBall::reset_to_rest() {
    position = prevRest;
    velocity = sf::Vector2f();
}
