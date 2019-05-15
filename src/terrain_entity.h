#pragma once

/*
 * Level.
 */

#include <string>
#include "entity.h"

enum TerrainType {
    T_BLANK,
    T_SOLID,
    T_KILL,
    T_SLOW,
    T_BOUNCY,
    T_STICKY,
    T_WIN,
};

class EntityTerrain : public Entity {
public:
    EntityTerrain(std::string filename, sf::Color colour);
    ~EntityTerrain();

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

    template <typename T>
    bool pos_in_bounds(T pos)
    {
        return !(pos.x < 0 || pos.x >= size.x || pos.y < 0 || pos.y >= size.y);
    }

    sf::Vector2u get_size();
    bool get_solid(sf::Vector2f pos);
    TerrainType get_pos(sf::Vector2f pos);
    sf::Vector2f get_normal(sf::Vector2f pos); // get surface normal (unit)
    bool intersects_with_circle(sf::Vector2f pos, float rad, sf::Vector2f *contact);
    bool loaded_successfuly();

    sf::Vector2f playerSpawn;
    sf::Color colour;

private:
    bool error = false;
    std::string filename;
    sf::Vector2u size; // terrain texture size
    sf::Shader shdTerrain;
    sf::Uint8 *terrain; // terrain data
    sf::Texture txtTerrainData; // terrain data as a texture (updated at draw time)
    // terrain textures that are drawn
    sf::Texture txtSolid;
    sf::Texture txtWater;
    sf::Texture txtBouncy;
    sf::Texture txtSlow;
    sf::Texture txtSticky;
    sf::Texture txtFinish;
    bool render = true;
    bool editMode = false;
    bool drawTerrainNormals = false;
    sf::Sprite sprTerrain;
};
