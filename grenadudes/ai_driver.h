#pragma once

// ai dude driver

#include "dude_driver.h"
#include "dude_entity.h"
#include "subject.h"
#include <vector>
#include "terrain_grid.h"
#include "nav_graph.h"

#define AI_GRENADE_FLEE_DIST 150.f
#define AI_SEEK_PLAYER_DIST 300.f

class EntityTerrain;

enum AiState {
    AI_STATE_IDLE, // doing nothing
    AI_STATE_FLEE, // fleeing a nearby grenade
    AI_STATE_SEEK, // seeking a strategic position
    AI_STATE_ATTACK, // attacking player
    AI_STATE_SEEK_PLAYER
};

class AiDriver : public DudeDriver, public Subject, public Observer {
public:
    AiDriver(EntityDude *dude, EntityTerrain *terrain);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

    void do_ai(std::vector<Entity*> &entities);

    void AiDriver::on_notify(Event event, void *data);
private:
    EntityTerrain *terrain;

    // terrain grid approximation
    TerrainGrid terrainGrid;
    bool showGrid;

    // nav graph
    NavGraph navGraph;
    bool showNav;

    // pathfinding
    std::vector<sf::Vector3f> currentPath;
    sf::Vector2f prevPosition;
    sf::Clock stuckTimer;
    sf::Clock jumpTimer;
    float jumpTimerTrigger;
    bool moveHor;

    // recalculating approximations
    sf::Clock recalcGridAndNodesTimer;
    sf::Clock recalcEdgesTimer;
    bool terrainChanged;
    bool recalcEdges;

    // decisions
    bool active;
    void change_state(AiState newState);
    AiState state;
    Entity *grenade;
    Entity* get_cloeset_grenade(std::vector<Entity*> &entities);
    sf::Clock recalcPath;
};