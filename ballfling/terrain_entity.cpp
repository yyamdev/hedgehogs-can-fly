#include "terrain_entity.h"
#include "build_options.h"
#include "util.h"
#include <vector>
#include <random>
#include <iostream>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "world.h"
#include "debug_draw.h"
#include "imgui.h"
#include <algorithm>

EntityTerrain::EntityTerrain(float scale, std::string filename, sf::Color colour) {
    this->colour = colour;
    // load map image
    sf::Image imgMap;
    if (!imgMap.loadFromFile(filename)) {
        error = true;
        return;
    }
    this->filename = filename;
    // properties
    size = imgMap.getSize();
    this->scale = scale;
    tag = "terrain";
    render = true;
    // allocate terrain data memory & load map into it
    terrain = new sf::Uint8[size.x * size.y * 4]; // 4 x 8 bit colour components for each pixel
    memcpy((void*)terrain, (void*)imgMap.getPixelsPtr(), (size_t)(size.x * size.y * 4));
    txtTerrainData.create(size.x, size.y); // for sfml rendering (passed to shader)
    data_pass();
    txtTerrainData.update(terrain); // update data texture
    // load front-end textures
    txtSolid.loadFromFile("data/solid.png");
    txtSolid.setRepeated(true);
    txtWater.loadFromFile("data/water.png");
    txtWater.setRepeated(true);
    txtBouncy.loadFromFile("data/bouncy.png");
    txtBouncy.setRepeated(true);
    txtSlow.loadFromFile("data/slow.png");
    txtSlow.setRepeated(true);
    txtSticky.loadFromFile("data/sticky.png");
    txtSticky.setRepeated(true);
    txtFinish.loadFromFile("data/finish.png");
    txtFinish.setRepeated(true);
    // load fragment shader
    shdTerrain.loadFromFile("data/terrain.frag", sf::Shader::Fragment);
    editMode = false;
    drawTerrainNormals = false;
}

void EntityTerrain::data_pass() {
    // iterate though map and spawn/store locations of entities
    // erase data pixels & comments
    for (unsigned int i=0; i< size.x * size.y * 4; i += 4) {
        if (terrain[i + 0] == 255 &&
            terrain[i + 1] == 127 &&
            terrain[i + 2] == 1) {
            // player
            playerSpawn.x = (float)((i / 4) % size.x);
            playerSpawn.y = (float)((i / 4) / size.x);
            // erase
            terrain[i + 0] = 0;
            terrain[i + 1] = 0;
            terrain[i + 2] = 0;
        }
        else if (terrain[i + 0] == 239 &&
                 terrain[i + 1] == 228 &&
                 terrain[i + 2] == 176) {
            // erase comment
            terrain[i + 0] = 0;
            terrain[i + 1] = 0;
            terrain[i + 2] = 0;
        }
    }
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
    if (solid) { // set to white (solid)
        terrain[base + 0] = 255;
        terrain[base + 1] = 255;
        terrain[base + 2] = 255;
        terrain[base + 3] = 255;
    } else { // set to black (empty)
        terrain[base + 0] = 0;
        terrain[base + 1] = 0;
        terrain[base + 2] = 0;
        terrain[base + 3] = 255;
    }
}

void EntityTerrain::remove_flood_fill(sf::Vector2f pos) {
    unsigned int base = (unsigned int)(((unsigned int)pos.y * size.x + (unsigned int)pos.x) * 4);
    flood_fill(pos, sf::Color::Black, sf::Color(terrain[base], terrain[base + 1], terrain[base + 2], 255));
}

void EntityTerrain::flood_fill(sf::Vector2f pos, sf::Color replacement, sf::Color old) {
    // may crash if near edge of map
    if (replacement == old) return;
    unsigned int base = (unsigned int)(((unsigned int)pos.y * size.x + (unsigned int)pos.x) * 4);
    if (old != sf::Color(terrain[base], terrain[base + 1], terrain[base + 2], 255)) return;
    terrain[base] = replacement.r;
    terrain[base + 1] = replacement.g;
    terrain[base + 2] = replacement.b;
    flood_fill(pos + sf::Vector2f(1.f, 0.f), replacement, old);
    flood_fill(pos + sf::Vector2f(-1.f, 0.f), replacement, old);
    flood_fill(pos + sf::Vector2f(0.f, 1.f), replacement, old);
    flood_fill(pos + sf::Vector2f(0.f, -1.f), replacement, old);
    return;
}

bool EntityTerrain::get_solid(sf::Vector2f pos) {
    if (!pos_in_bounds(pos))
        return false;
    unsigned int base = (unsigned int)(((unsigned int)pos.y * size.x + (unsigned int)pos.x) * 4);
    return !(terrain[base + 0] == 0 &&
             terrain[base + 1] == 0 &&
             terrain[base + 2] == 0)
            &&
           !(terrain[base + 0] == 0 &&
             terrain[base + 1] == 128 &&
             terrain[base + 2] == 128)
            &&
           !(terrain[base + 0] == 185 &&
             terrain[base + 1] == 122 &&
             terrain[base + 2] == 87)
            &&
           !(terrain[base + 0] == 255 &&
             terrain[base + 1] == 242 &&
             terrain[base + 2] == 0);
}

TerrainType EntityTerrain::get_pos(sf::Vector2f pos) {
    if (!pos_in_bounds(pos))
        return T_BLANK;
    unsigned int base = (unsigned int)(((unsigned int)pos.y * size.x + (unsigned int)pos.x) * 4);
    if (terrain[base + 0] == 255 &&
        terrain[base + 1] == 255 &&
        terrain[base + 2] == 255) return T_SOLID;

    if (terrain[base + 0] == 0 &&
        terrain[base + 1] == 128 &&
        terrain[base + 2] == 128) return T_KILL;

    if (terrain[base + 0] == 67 &&
        terrain[base + 1] == 191 &&
        terrain[base + 2] == 6) return T_BOUNCY;

    if (terrain[base + 0] == 210 &&
        terrain[base + 1] == 176 &&
        terrain[base + 2] == 60) return T_SLOW;

    if (terrain[base + 0] == 255 &&
        terrain[base + 1] == 174 &&
        terrain[base + 2] == 201) return T_STICKY;

    if (terrain[base + 0] == 255 &&
        terrain[base + 1] == 242 &&
        terrain[base + 2] == 0) return T_THIN;

    if (terrain[base + 0] == 237 &&
        terrain[base + 1] == 28 &&
        terrain[base + 2] == 36) return T_WIN;

    return T_BLANK;
}

void EntityTerrain::set_solid() {
    for (unsigned int i=0; i<size.x * size.y * 4; i+=4) {
        // set to white (solid)
        terrain[i + 0] = 255;
        terrain[i + 1] = 255;
        terrain[i + 2] = 255;
        terrain[i + 3] = 255;
    }
}

void EntityTerrain::set_empty() {
    for (unsigned int i=0; i<size.x * size.y * 4; i+=4) {
        // set to black (empty)
        terrain[i + 0] = 0;
        terrain[i + 1] = 0;
        terrain[i + 2] = 0;
        terrain[i + 3] = 255;
    }
}

void EntityTerrain::set_rectangle(sf::FloatRect rect, bool solid) {
    for (float y=rect.top; y<rect.top + rect.height; ++y) {
        for (float x=rect.left; x<rect.left + rect.width; ++x) {
            set_solid(sf::Vector2f(x, y), solid);
        }
    }
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
}

sf::Vector2f EntityTerrain::get_normal(sf::Vector2f pos) {
    std::vector<sf::Vector2f> positions;
    float sampleRadius = 4;
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

bool EntityTerrain::intersects_with_circle(sf::Vector2f pos, sf::Vector2f vel, float rad, sf::Vector2f *contact, sf::Vector2f *newPos) {
    // broad phase
    bool intersects = false;
    int divisions = 16;
    sf::Vector2f contactBroad;
    float deltaAngle = (2.f * (float)M_PI) / (float)divisions;
    for (int i=0; i<divisions; ++i) {
        float angle = (float)i * deltaAngle;
        sf::Vector2f probe(pos.x + rad * cos(angle), pos.y + rad * sin(angle));
        if (get_solid(probe)) {
            intersects = true;
            contactBroad = probe;
            break; // circle definitely intersects
        }
    }
    if (!intersects) return false;
    else { // narrow phase (find contact point)
        // this is a load of shit really... half of it probably isn't necessary
        // doesn't account for multiple contact points so doesn't work well in tight scenarios
        bool inside = true;
        bool once = true;
        *newPos = pos;
        *contact = contactBroad;
        float speed = fmax(1.f, util::len(vel));
        vel = util::normalize(vel * speed);
        if (util::len(vel) != 0.f){
            sf::Vector2f velUnit = util::normalize(vel);
            int max = 40;
            int i = 0;
            while (inside && i < max) {
                ++i;
                inside = false;
                int divisions = 16;
                float deltaAngle = (2.f * (float)M_PI) / (float)divisions;
                for (int i=0; i<divisions; ++i) {
                    float angle = (float)i * deltaAngle;
                    sf::Vector2f probe(newPos->x + rad * cos(angle), newPos->y + rad * sin(angle));
                    if (get_solid(probe)) {
                        *contact = probe;
                        *newPos -= velUnit;
                        inside = true;
                        once = false;
                        break;
                    }
                }
            }
            if (once) {
                *newPos -= vel;
            }
        }
        return true;
    }
}

void EntityTerrain::event(sf::Event &e) {
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Right && !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
        //std::cout << "TODO -> EVENT_TERRAIN_CHANGED if this code uses set_circle\n";
        //set_circle(sf::Vector2f((float)e.mouseButton.x + world->camera.x, (float)e.mouseButton.y + world->camera.y), 25, false);
    }
}

void EntityTerrain::tick(std::vector<Entity*> &entities) {
    world->set_camera_clamp(sf::Vector2f(0.f, 0.f), sf::Vector2f((float)size.x, (float)size.y));
}

void EntityTerrain::draw(sf::RenderWindow &window) {
    static bool drawGrid = false;
    if (render) {
        // create terrain image sprite
        sf::Sprite sprTerrain(txtSolid); // I think this has to be set for size only?
        sprTerrain.setTextureRect(sf::IntRect(0, 0, (int)size.x, (int)size.y));
        // pass parameters into shader
        shdTerrain.setParameter("txtSolid", txtSolid);
        shdTerrain.setParameter("txtKill", txtWater);
        shdTerrain.setParameter("txtBouncy", txtBouncy);
        shdTerrain.setParameter("txtSlow", txtSlow);
        shdTerrain.setParameter("txtSticky", txtSticky);
        shdTerrain.setParameter("txtFinish", txtFinish);
        shdTerrain.setParameter("txtData", txtTerrainData);
        shdTerrain.setParameter("sizeX", (float)size.x);
        shdTerrain.setParameter("sizeY", (float)size.y);
        shdTerrain.setParameter("screenWidth", (float)WINDOW_WIDTH);
        shdTerrain.setParameter("screenHeight", (float)WINDOW_HEIGHT);
        shdTerrain.setParameter("cameraX", world->camera.x);
        shdTerrain.setParameter("cameraY", world->camera.y);
        shdTerrain.setParameter("grid", drawGrid);
        shdTerrain.setParameter("edgeColour", colour);
        sprTerrain.setPosition(-world->camera);
        window.draw(sprTerrain, &shdTerrain); // draw
    }

    if (edit && ImGui::CollapsingHeader("Terrain")) {
        ImGui::TextDisabled(filename.c_str());
        ImGui::Checkbox("render", &render);
        ImGui::Checkbox("grid effect", &drawGrid);
        ImGui::Checkbox("Draw terrain normals", &drawTerrainNormals);
    }

    if (drawTerrainNormals) {
        sf::Vector2i mouseI = sf::Mouse::getPosition(window);
        sf::Vector2f mouse;
        mouse.x = (float)mouseI.x;
        mouse.y = (float)mouseI.y;
        sf::Vector2f normal = get_normal(mouse + world->camera);
        draw_vector(mouse, normal, 50.f, sf::Color::Yellow, window);
    }

}

void EntityTerrain::on_notify(Event event, void *data) {
    if (event == EVENT_TERRAIN_CHANGE) {
        // update part of the terrain texture from terrain data data
        sf::Rect<unsigned int> updateBounds = *(sf::Rect<unsigned int>*)data;
        sf::Uint8 *terrainUpdate = new sf::Uint8[updateBounds.width * updateBounds.height * 4];
        sf::Uint8 *terrainStart = &terrain[(updateBounds.top * size.x + updateBounds.left) * 4];
        for (int row = 0; row < (int)updateBounds.height; ++row) { // create continuous buffer for update reigion
            memcpy( &terrainUpdate[row * updateBounds.width * 4],
                    &terrainStart [row * size.x * 4],
                    updateBounds.width * 4);
        }
        txtTerrainData.update(terrainUpdate, updateBounds.width, updateBounds.height, updateBounds.left, updateBounds.top); // update data texture
        delete[] terrainUpdate;
    }
}
