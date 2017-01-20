#include "terrain_entity.h"
#include "build_options.h"
#include "util.h"
#include <vector>
#include <random>
#include <iostream>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>

EntityTerrain::EntityTerrain(sf::Vector2u size, float scale) {
    tag = "terrain";
    // store shit
    this->size = size;
    this->scale = scale;
    // allocate terrain data memory
    terrain = new sf::Uint8[size.x * size.y * 4]; // each color component
    set_solid();
    // create empty terrain data texture
    txtTerrainData.create(size.x, size.y);
    // load terrain image texture
    txtImage.loadFromFile("data/terrain.png");
    txtImage.setRepeated(true);
    // load terrain fragment shader
    shdTerrain.loadFromFile("data/terrain.frag", sf::Shader::Fragment);
    render = true;
    notify(EVENT_TERRAIN_CHANGE, NULL);
}

EntityTerrain::~EntityTerrain() {
    // free terrain data memory
    delete[] terrain;
}

sf::Vector2u EntityTerrain::get_size() {
    return size;
}

void EntityTerrain::set_solid(sf::Vector2f pos, bool solid) {
    if (!pos_in_bounds(pos))
        return;
    unsigned int base = (unsigned int)(((unsigned int)pos.y * size.x + (unsigned int)pos.x) * 4);
    if (solid) {
        // set to white (solid)
        terrain[base + 0] = 255;
        terrain[base + 1] = 255;
        terrain[base + 2] = 255;
        terrain[base + 3] = 255;
    } else {
        // set to black (empty)
        terrain[base + 0] = 0;
        terrain[base + 1] = 0;
        terrain[base + 2] = 0;
        terrain[base + 3] = 255;
    }
    notify(EVENT_TERRAIN_CHANGE, NULL);
}

bool EntityTerrain::get_solid(sf::Vector2f pos) {
    if (!pos_in_bounds(pos))
        return false;
    unsigned int base = (unsigned int)(((unsigned int)pos.y * size.x + (unsigned int)pos.x) * 4);
    return (terrain[base + 0] == 255 &&
            terrain[base + 1] == 255 &&
            terrain[base + 2] == 255);
}

void EntityTerrain::set_solid() {
    for (unsigned int i=0; i<size.x * size.y * 4; i+=4) {
        // set to white (solid)
        terrain[i + 0] = 255;
        terrain[i + 1] = 255;
        terrain[i + 2] = 255;
        terrain[i + 3] = 255;
    }
    notify(EVENT_TERRAIN_CHANGE, NULL);
}

void EntityTerrain::set_empty() {
    for (unsigned int i=0; i<size.x * size.y * 4; i+=4) {
        // set to black (empty)
        terrain[i + 0] = 0;
        terrain[i + 1] = 0;
        terrain[i + 2] = 0;
        terrain[i + 3] = 255;
    }
    notify(EVENT_TERRAIN_CHANGE, NULL);
}

void EntityTerrain::set_rectangle(sf::FloatRect rect, bool solid) {
    for (float y=rect.top; y<rect.top + rect.height; ++y) {
        for (float x=rect.left; x<rect.left + rect.width; ++x) {
            set_solid(sf::Vector2f(x, y), solid);
        }
    }
    notify(EVENT_TERRAIN_CHANGE, NULL);
}

void EntityTerrain::set_circle(sf::Vector2f center, float rad, bool solid) {
    sf::FloatRect bound(center.x - rad, center.y - rad, rad * 2.f, rad * 2);
    for (float y=bound.top; y<bound.top + bound.height; ++y) {
        for (float x=bound.left; x<bound.left + bound.width; ++x) {
            float dx = center.x - x;
            float dy = center.y - y;
            if ((dx * dx + dy * dy) < (rad * rad))
                set_solid(sf::Vector2f(x, y), solid);
        }
    }
    notify(EVENT_TERRAIN_CHANGE, NULL);
}

sf::Vector2f EntityTerrain::get_normal(sf::Vector2f pos) {
    std::vector<sf::Vector2f> positions;
    float sampleRadius = 5;
    for (float y=-sampleRadius; y<sampleRadius + 1; ++y) {
        for (float x=-sampleRadius; x<sampleRadius + 1; ++x) {
            sf::Vector2f p(pos.x + x, pos.y + y);
            if (pos_in_bounds(p) && get_solid(sf::Vector2f(p.x, p.y)))
                positions.push_back(p);
        }
    }

    sf::Vector2f av;
    for (auto &p : positions) {
        av += p;
    }
    if (positions.size() != 0)
        av /= (float)positions.size();

    sf::Vector2f normal = sf::Vector2f(pos.x, pos.y) - av;
    float normalSize = util::distance(0.f, 0.f, normal.x, normal.y);
    if (normalSize == 0.f) normalSize = 1.f;
    return (normal / normalSize);
}

sf::Vector2f EntityTerrain::get_normal_ground(sf::Vector2f pos) {
    while (!get_solid(pos)) {
        pos.y += 1;
    }
    return get_normal(pos);
}

bool EntityTerrain::intersects_with_circle(sf::Vector2f pos, float rad, sf::Vector2f *contact, int divisions) {
    bool intersects = false;
    float deltaAngle = (2.f * (float)M_PI) / (float)divisions;
    for (int i=0; i<divisions; ++i) {
        float angle = (float)i * deltaAngle;
        sf::Vector2f probe(pos.x + rad * cos(angle), pos.y + rad * sin(angle));
        if (get_solid(probe)) {
            if (contact)
                *contact = probe;
            return true;
        }
    }
    return false;
}

bool EntityTerrain::intersects_with_circle(sf::Vector2f pos, float rad, sf::Vector2f *contact) {
    return intersects_with_circle(pos, rad, contact, 8);
}

void EntityTerrain::event(sf::Event &e) {
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Right && !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
        set_circle(sf::Vector2f((float)e.mouseButton.x, (float)e.mouseButton.y), 70, false);
        //set_rectangle(sf::FloatRect((float)e.mouseButton.x, (float)e.mouseButton.y, 256.f, 64.f), false);
    }
    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::T)
        render = !render;
}

void EntityTerrain::tick(std::vector<Entity*> &entities) {
}

void EntityTerrain::draw(sf::RenderWindow &window) {
    if (!render) return;
    txtTerrainData.update(terrain); // update data texture
    // create terrain image sprite
    sf::Sprite sprTerrain(txtImage);
    sprTerrain.setTextureRect(sf::IntRect(0, 0, (int)size.x, (int)size.y));
    // pass parameters into shader
    shdTerrain.setParameter("txtTerrain", txtImage);
    shdTerrain.setParameter("txtData", txtTerrainData);
    shdTerrain.setParameter("sizeX", (float)size.x);
    shdTerrain.setParameter("sizeY", (float)size.y);
    shdTerrain.setParameter("screenHeight", (float)WINDOW_HEIGHT);
    window.draw(sprTerrain, &shdTerrain); // draw
}

void EntityTerrain::generate_flat() {
    set_solid();
    set_rectangle(sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f((float)WINDOW_WIDTH, 128.f)), false);
    notify(EVENT_TERRAIN_CHANGE, NULL);
}

void EntityTerrain::generate_normal(float yMax, float yMin) {
    set_empty();

    float yLast = (yMax + yMin) / 2.f;
    std::vector<float> height;
    std::default_random_engine generator;
    generator.seed((unsigned int)time(NULL));
    std::normal_distribution<float> distribution(0.f, 0.7f);
    int directionCount = 1;
    int directionCountMax = 16;
    float delta = distribution(generator);
    for (unsigned int x=0; x<size.x; ++x) {
        height.push_back(yLast);
        yLast += delta;
        yLast = util::clamp(yLast, yMin, yMax);
        directionCount = (directionCount + 1) % directionCountMax;
        if (directionCount == 0)
            delta = distribution(generator);
    }

    for (unsigned int x=0; x<size.x; ++x) {
        set_rectangle(sf::FloatRect((float)x, (float)WINDOW_HEIGHT - height[x], 1.f, (float)WINDOW_HEIGHT), true);
        //std::cout << height[x] << std::endl;
    }

    notify(EVENT_TERRAIN_CHANGE, NULL);
}