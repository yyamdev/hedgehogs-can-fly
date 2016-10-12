#include "terrain_grid.h"
#include "terrain_entity.h"

TerrainGrid::TerrainGrid(EntityTerrain *terrain, unsigned int cellSize) {
    this->terrain = terrain;
    this->cellSize = cellSize;
    init_grid();
}

void TerrainGrid::init_grid() {
    gridWidth = terrain->get_size().x / cellSize;
    gridHeight = terrain->get_size().y / cellSize;
    for (unsigned int y = 0; y<gridHeight; ++y) {
        for (unsigned int x = 0; x<gridWidth; ++x) {
            grid.push_back(true);
        }
    }
}

unsigned int TerrainGrid::get_grid_width(){ return gridWidth; }
unsigned int TerrainGrid::get_grid_height(){ return gridHeight; }
unsigned int TerrainGrid::get_cell_size(){ return cellSize; }

unsigned int TerrainGrid::get_index(sf::Vector2u position) {
    return position.x + (position.y * gridWidth);
}

sf::Vector2u TerrainGrid::get_position(unsigned int index) {
    sf::Vector2u position;
    position.x = index % gridWidth;
    position.y = index / gridHeight;
    return position;
}

bool TerrainGrid::index_valid(unsigned int index) {
    if (index < grid.size())
        return true;
    return false;
}

bool TerrainGrid::position_valid(sf::Vector2u position) {
    return index_valid(get_index(position));
}

bool TerrainGrid::is_solid(unsigned int index) {
    if (!index_valid(index))
        return true;
    return grid[index];
}

void TerrainGrid::recalc_pos(sf::Vector2u position) {
    if (!position_valid(position))
        return;
    unsigned int solids = 0;
    for (unsigned int sampleY = 0; sampleY<cellSize; ++sampleY) {
        for (unsigned int sampleX = 0; sampleX<cellSize; ++sampleX) { // sample reigion
            float xPosTerrain = (float)(position.x * cellSize + sampleX);
            float yPosTerrain = (float)(position.y * cellSize + sampleY);
            if (terrain->get_solid(sf::Vector2f(xPosTerrain, yPosTerrain)))
                ++solids;
        }
    }
    if (solids > (cellSize * cellSize) / 2)
        grid[get_index(position)] = true;
    else
        grid[get_index(position)] = false;
}

void TerrainGrid::recalc_index(unsigned int index) {
    recalc_pos(get_position(index));
}

void TerrainGrid::recalc_all() {
    for (unsigned int y = 0; y<gridHeight; ++y) {
        for (unsigned int x = 0; x<gridWidth; ++x) {
            recalc_pos(sf::Vector2u(x, y));
        }
    }
}

void TerrainGrid::recalc_rect(sf::Rect<unsigned int> rect) {
    for (unsigned int y = rect.top; y<rect.top + rect.height; ++y) {
        for (unsigned int x = rect.left; x<rect.left + rect.width; ++x) {
            recalc_pos(sf::Vector2u(x, y));
        }
    }
}

void TerrainGrid::draw(sf::RenderWindow &window) {
    unsigned int index = 0;
    for (unsigned int y = 0; y<gridHeight; ++y) {
        for (unsigned int x = 0; x<gridWidth; ++x) {
            if (index < grid.size() && grid[index]) {
                sf::RectangleShape rect(sf::Vector2f((float)cellSize, (float)cellSize));
                rect.setFillColor(sf::Color(255, 0, 0, 128));
                rect.setPosition(sf::Vector2f((float)(x * cellSize), (float)(y * cellSize)));
                window.draw(rect);
            }
            ++index;
        }
    }
}