#pragma once

// grid approximation of terrain data

#include <vector>
#include <SFML/Graphics.hpp>

class EntityTerrain;

class TerrainGrid {
public:
    TerrainGrid(EntityTerrain *terrain, unsigned int cellSize);

    // all functions operate on 'grid' member

    unsigned int get_grid_width();
    unsigned int get_grid_height();
    unsigned int get_cell_size();

    bool index_valid(unsigned int index);
    bool position_valid(sf::Vector2u position);
    unsigned int get_index(sf::Vector2u position);
    sf::Vector2u get_position(unsigned int index);

    void recalc_pos(sf::Vector2u position);
    void recalc_index(unsigned int index);
    void recalc_all();
    void recalc_rect(sf::Rect<unsigned int> rect);

    void draw(sf::RenderWindow &window);

    bool is_solid(unsigned int index);
    std::vector<bool> grid;
private:
    void init_grid();

    EntityTerrain *terrain;
    unsigned int cellSize;
    unsigned int gridWidth, gridHeight;
};