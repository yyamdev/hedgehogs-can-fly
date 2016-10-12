#include "ai_driver.h"
#include "terrain_entity.h"
#include "util.h"
#include <iostream>

AiDriver::AiDriver(EntityDude *dude, EntityTerrain *terrain) : terrainGrid(terrain, 16), DudeDriver(dude) {
    this->terrain = terrain;
    quad = 0;
    showGrid = false;
    state = AI_STATE_IDLE;
    Subject::add_observer(this);
}

AiDriver::~AiDriver() {
    Subject::remove_observer(this);
}

void AiDriver::event(sf::Event &e) {
    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::G)
        showGrid = !showGrid;
    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::N)
        showNav = !showNav;
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Middle) {
        sf::Vector2f mouse;
        mouse.x = (float)e.mouseButton.x;
        mouse.y = (float)e.mouseButton.y;
        NavNode *dest = get_closest_node(navGraph, mouse);
        find_path(get_closest_node(navGraph, dude->position), dest, navGraph, &currentPath);
    }
}

void AiDriver::tick(std::vector<Entity*> &entities) {
    // find player
    EntityDude *player = NULL;
    for (auto &entity : entities) {
        if (entity->get_tag() == "dude") {
            EntityDude * entityDude = (EntityDude*)entity;
            if (entityDude->get_number() == PLAYER_NUMBER)
                player = entityDude;
        }
    }

    // decide what to do
    Entity *closestGrenade = get_cloeset_grenade(entities);
    if (closestGrenade && util::len(dude->position - closestGrenade->position) < AI_GRENADE_FLEE_DIST) {
        state = AI_STATE_FLEE;
        grenade = closestGrenade;
    } else if (state != AI_STATE_ATTACK) {
        // seek player
        state = AI_STATE_SEEK_PLAYER;
    }

    // flee grenade
    if (state == AI_STATE_FLEE) {
        if (!grenade)
            state = AI_STATE_IDLE;
        else {
            NavNode *nodeRight = get_closest_node(navGraph, grenade->position + sf::Vector2f(AI_GRENADE_FLEE_DIST, 0.f));
            NavNode *nodeLeft = get_closest_node(navGraph, grenade->position - sf::Vector2f(AI_GRENADE_FLEE_DIST, 0.f));
            NavNode *choice = NULL;
            // move in opposite direction to grenade velocity
            if (dude->position.x - grenade->position.x > 0.f)
                choice = nodeRight;
            else
                choice = nodeLeft;
            find_path(get_closest_node(navGraph, dude->position), choice, navGraph, &currentPath);
        }
    }

    // attack player
    if (state == AI_STATE_ATTACK) {
        if (player) {
            float xDif = player->position.x - dude->position.x;
            dude->throw_grenade(sf::Vector2f(util::sign(xDif), -1.f), std::fabs(xDif) / 60.f);
            state = AI_STATE_IDLE;
        }
    }

    // seek player
    if (state == AI_STATE_SEEK_PLAYER && player) {
        if (recalcPlayerPath.getElapsedTime().asSeconds() > 1.f) {
            NavNode *nodePlayer = get_closest_node(navGraph, player->position);
            find_path(get_closest_node(navGraph, dude->position), nodePlayer, navGraph, &currentPath);
            recalcPlayerPath.restart();
        }
        if (util::len(player->position - dude->position) < AI_SEEK_PLAYER_DIST) {
            state = AI_STATE_ATTACK;
        }
    }

    // recalculate terrain approximation & nav graph
    if (clockRecalcTerrainGrid.getElapsedTime().asSeconds() > 0.5f) {
        unsigned int halfWidth = terrainGrid.get_grid_width() / 2;
        unsigned int halfHeight = terrainGrid.get_grid_height() / 2;
        switch (quad) {
        case 0:
            terrainGrid.recalc_rect(sf::Rect<unsigned int>(0, 0, halfWidth, halfHeight));
            break;
        case 1:
            terrainGrid.recalc_rect(sf::Rect<unsigned int>(halfWidth, 0, halfWidth, halfHeight));
            break;
        case 2:
            terrainGrid.recalc_rect(sf::Rect<unsigned int>(0, halfHeight, halfWidth, halfHeight));
            break;
        case 3:
            terrainGrid.recalc_rect(sf::Rect<unsigned int>(halfWidth, halfHeight, halfWidth, halfHeight));
            break;
        }
        quad = (quad + 1) % 4;
        navGraph = generate_nav_graph(terrainGrid);
        clockRecalcTerrainGrid.restart();
    }

    // follow path
    if (currentPath.size() != 0) {
        if (std::fabs(dude->position.x - currentPath.back().x) < 8.f)
            currentPath.pop_back();
        else {
            if (dude->position.x - currentPath.back().x > 0.f)
                dude->move(EntityDude::DIRECTION_LEFT);
            else
                dude->move(EntityDude::DIRECTION_RIGHT);
            if (currentPath.back().z == 1.f)
                dude->jump();
        }
    }

    if (dude->position != prevPosition)
        stuckTimer.restart();
    if (stuckTimer.getElapsedTime().asSeconds() > 0.25f && currentPath.size() != 0) {
        dude->jump(); // jump if stuck for more than 1/4 second
    }

    prevPosition = dude->position;
}

void AiDriver::draw(sf::RenderWindow &window) {
    if (showGrid) // draw terrain grid
        terrainGrid.draw(window);

    if (showNav) // draw nav graph
        draw_nav_graph(window, navGraph);
}

Entity* AiDriver::get_cloeset_grenade(std::vector<Entity*> &entities) {
    Entity *closest = NULL;
    float closestDist = 1000000.f;
    for (auto &entity : entities) {
        if (entity->get_tag() == "grenade") {
            float dist = util::len(dude->position - entity->position);
            if (dist < closestDist) {
                closestDist = dist;
                closest = entity;
            }
        }
    }
    return closest;
}

void AiDriver::on_notify(Event event, void *data) {
    if (event == EVENT_GRENADE_EXPLODE) {
        Entity *gren = (Entity*)data;
        if (state == AI_STATE_FLEE) {
            if (gren == grenade)
                grenade = NULL;
        }
    }
}