#pragma once

// destructable level

#include "entity.h"
#include "subject.h"

class EntityTerrain : public Entity {
public:
    EntityTerrain(sf::Vector2u size, float scale);
    ~EntityTerrain(); // free all memory

    template <typename T>
    bool pos_in_bounds(T pos) {
        return !(pos.x < 0 || pos.x >= size.x || pos.y < 0 || pos.y >= size.y);
    }

    sf::Vector2u get_size();

    // basic manipulation
    void set_solid(sf::Vector2f pos, bool solid);
    bool get_solid(sf::Vector2f pos);

    void set_solid(); // make terrain all solid
    void set_empty(); // make terrain all empty
    
    void set_rectangle(sf::FloatRect rect, bool solid); // set a rectangular block
    void set_circle(sf::Vector2f center, float rad, bool solid); // set a circular block

    sf::Vector2f get_normal(sf::Vector2f pos); // get surface normal (unit)
    sf::Vector2f get_normal_ground(sf::Vector2f pos); // get surface normal (unit) at nearest downwards ground

    // intersection tests
    bool intersects_with_circle(sf::Vector2f pos, float rad, sf::Vector2f *contact);
    bool intersects_with_circle(sf::Vector2f pos, float rad, sf::Vector2f *contact, int divisions);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);
    
    // generation
    void generate_flat();
    void generate_normal(float yMax, float yMin);

private:
    sf::Vector2u size; // terrain texture size
    float scale; // scale factor for drawing
    sf::Shader shdTerrain;
    sf::Texture txtImage; // terrain texture that is drawn
    sf::Uint8 *terrain; // terrain data
    sf::Texture txtTerrainData; // terrain data as a texture (updated at draw time)
    bool render;
};