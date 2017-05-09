#pragma once

// destructable level

#include "entity.h"
#include "subject.h"
#include <string>

enum TerrainType {
    T_BLANK,
    T_SOLID,
    T_KILL,
    T_SLOW,
    T_BOUNCY,
    T_STICKY,
    T_WEAK,
    T_THIN,
    T_WIN,
};

class EntityTerrain : public Entity, public Observer {
public:
    EntityTerrain(float scale, std::string filename);
    ~EntityTerrain(); // free all memory

    void on_notify(Event event, void *data);

    template <typename T>
    bool pos_in_bounds(T pos) {
        return !(pos.x < 0 || pos.x >= size.x || pos.y < 0 || pos.y >= size.y);
    }

    sf::Vector2u get_size();

    // basic manipulation
    void set_solid(sf::Vector2f pos, bool solid);
    bool get_solid(sf::Vector2f pos);
    TerrainType get_pos(sf::Vector2f pos);

    void remove_flood_fill(sf::Vector2f pos);
    void flood_fill(sf::Vector2f pos, sf::Color replacement, sf::Color old);

    void set_solid(); // make terrain all solid
    void set_empty(); // make terrain all empty
    
    void set_rectangle(sf::FloatRect rect, bool solid); // set a rectangular block
    void set_circle(sf::Vector2f center, float rad, bool solid); // set a circular block
    void set_weak_terrain_circle(sf::Vector2f center, float rad, bool solid); // sets the weak terrain im a circular block

    sf::Vector2f get_normal(sf::Vector2f pos); // get surface normal (unit)
    sf::Vector2f get_normal_ground(sf::Vector2f pos); // get surface normal (unit) at nearest downwards ground

    // intersection tests
    bool intersects_with_circle(sf::Vector2f pos, sf::Vector2f vel, float rad, sf::Vector2f *contact, sf::Vector2f *newPos);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

    sf::Vector2f playerSpawn;

    bool error();

private:
    std::string filename;
    sf::Vector2u size; // terrain texture size
    float scale; // scale factor for drawing
    sf::Shader shdTerrain;
    sf::Uint8 *terrain; // terrain data
    sf::Texture txtTerrainData; // terrain data as a texture (updated at draw time)
    // terrain textures that are drawn
    sf::Texture txtBg;
    sf::Texture txtSolid;
    sf::Texture txtWater;
    sf::Texture txtWeak;
    sf::Texture txtBouncy;
    sf::Texture txtSlow;
    sf::Texture txtSticky;
    sf::Texture txtFinish;
    sf::Texture txtEdge;
    bool render;
    void data_pass();
    bool editMode;
    bool _error;
};