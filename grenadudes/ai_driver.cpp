#include "ai_driver.h"
#include "terrain_entity.h"
#include "util.h"
#include <iostream>

AiDriver::AiDriver(EntityDude *dude, EntityTerrain *terrain) : terrainGrid(terrain, 16), DudeDriver(dude) {
    this->terrain = terrain;
    showGrid = false;
    state = AI_STATE_IDLE;
    active = true; // ai on by default
    terrainChanged = true;
    recalcEdges = false;
    jumpTimerTrigger = -1.f;
}

void AiDriver::event(sf::Event &e) {
    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::G)
        showGrid = !showGrid;
    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::N)
        showNav = !showNav;
    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::P) {
        active = !active;
        if (!active)
            currentPath.clear();
    }
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Middle && !active) {
        sf::Vector2f mouse;
        mouse.x = (float)e.mouseButton.x;
        mouse.y = (float)e.mouseButton.y;
        NavNode *dest = get_closest_node(navGraph, mouse);
        find_path(get_closest_node(navGraph, dude->position), dest, navGraph, &currentPath);
    }
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Right && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
        // toggle mark nearest node
        sf::Vector2f mouseF = sf::Vector2f((float)e.mouseButton.x, (float)e.mouseButton.y);
        NavNode* node = get_closest_node(navGraph, mouseF);
        node->marked = !node->marked;
    }
}

void AiDriver::tick(std::vector<Entity*> &entities) {
    if (active)
        do_ai(entities);

    if (terrainChanged && recalcGridAndNodesTimer.getElapsedTime().asSeconds() > 1.f) {
        // recalc.
        terrainGrid.recalc_all();
        navGraph = generate_nav_graph_nodes(terrainGrid);

        // reset & trigger edge gen
        terrainChanged = false;
        recalcGridAndNodesTimer.restart();
        recalcEdges = true;
        recalcEdgesTimer.restart();
    }

    if (recalcEdges && recalcEdgesTimer.getElapsedTime().asSeconds() > 1.f) {
        generate_nav_graph_edges(navGraph, terrainGrid, false);
        recalcEdges = false;
    }

    // follow path
    /*
    if (currentPath.size() != 0) {
        if (std::fabs(dude->position.x - currentPath.back().x) < 8.f) {
            moveHor = false;
            if (std::fabs(dude->position.y - currentPath.back().y) < 16.f)
                currentPath.pop_back();
        }
        else {
            if (moveHor) {
                if (dude->position.x - currentPath.back().x > 0.f)
                    dude->move(EntityDude::DIRECTION_LEFT);
                else
                    dude->move(EntityDude::DIRECTION_RIGHT);
            }
            if (currentPath.back().z == 1.f) {
                dude->jump();
                // 1 second per square
                jumpTimer.restart();
                jumpTimerTrigger = 1.f * ((dude->position.y - currentPath.back().y) / (float)terrainGrid.get_cell_size());
                moveHor = false;
            } else
                moveHor = true;
        }
    }

    if (jumpTimer.getElapsedTime().asSeconds() > jumpTimerTrigger) {
        moveHor = true;
    }
    */
    
    // follow path
    if (currentPath.size() != 0) {
        if (currentPath.back().z == 0.f) { // move along walkable edge
            if (dude->position.x - currentPath.back().x > 0.f)
                dude->move(EntityDude::DIRECTION_LEFT);
            else
                dude->move(EntityDude::DIRECTION_RIGHT);
        }

        if (currentPath.back().z == 1.f) { // move along jumpable edge
            dude->jump();
            if (dude->position.x - currentPath.back().x > 0.f)
                dude->move(EntityDude::DIRECTION_LEFT);
            else
                dude->move(EntityDude::DIRECTION_RIGHT);
        }

        if (std::fabs(dude->position.x - currentPath.back().x) < 4.f /*&& 
            std::fabs(dude->position.y - currentPath.back().y) < 16.f*/) {
            // reached a target node
            std::cout << "next\n";
            currentPath.pop_back(); // change target to next node
            if (currentPath.size() > 0 && currentPath.back().z == 1.f) { // if next node needs to be jumped to
                //std::cout << "jump\n";
                //dude->jump();
            }
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
    if (event == EVENT_TERRAIN_CHANGE) {
        terrainChanged = true;
        recalcGridAndNodesTimer.restart();
    }
}

void AiDriver::change_state(AiState newState) {
    state = newState;
    notify(EVENT_AI_STATE_CHANGE, (void*)(&state));
}

void AiDriver::do_ai(std::vector<Entity*> &entities) {
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
        change_state(AI_STATE_FLEE);
        grenade = closestGrenade;
    } else
        change_state(AI_STATE_SEEK_PLAYER);

    // flee grenade
    if (state == AI_STATE_FLEE) {
        if (!grenade)
            change_state(AI_STATE_IDLE);
        else {
            NavNode *nodeRight = get_closest_node(navGraph, grenade->position + sf::Vector2f(AI_GRENADE_FLEE_DIST + 32.f, 0.f));
            NavNode *nodeLeft = get_closest_node(navGraph, grenade->position - sf::Vector2f(AI_GRENADE_FLEE_DIST + 32.f, 0.f));
            NavNode *choice = NULL;
            // move in opposite direction to grenade
            if (dude->position.x - grenade->position.x > 0.f)
                choice = nodeRight;
            else
                choice = nodeLeft;
            if (recalcPath.getElapsedTime().asSeconds() > 1.f) {
                find_path(get_closest_node(navGraph, dude->position), choice, navGraph, &currentPath);
                recalcPath.restart();
            }
        }
    }

    // seek player
    if (state == AI_STATE_SEEK_PLAYER && player) {
        if (recalcPath.getElapsedTime().asSeconds() > 1.f) {
            NavNode *nodePlayer = get_closest_node(navGraph, player->position);
            find_path(get_closest_node(navGraph, dude->position), nodePlayer, navGraph, &currentPath);
            recalcPath.restart();
        }
        if (util::len(player->position - dude->position) <= AI_SEEK_PLAYER_DIST) {
            change_state(AI_STATE_ATTACK);
        }
    }

    // attack player
    if (state == AI_STATE_ATTACK) {
        if (player) {
            float xDif = player->position.x - dude->position.x;
            dude->throw_grenade(sf::Vector2f(util::sign(xDif), -1.f), std::fabs(xDif) / 60.f + util::rnd(0.f, 2.f));
            change_state(AI_STATE_IDLE);
        }
    }
}