#include <assert.h>
#include <vector>
#include <random>
#include <iostream>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include "terrain_entity.h"
#include "build_options.h"
#include "util.h"
#include "world.h"
#include "debug_draw.h"
#include "imgui.h"
#include "options.h"

EntityTerrain::EntityTerrain(std::string filename, sf::Color colour) :
    colour(colour), filename(filename)
{
    tag = "terrain";

    // Load image
    sf::Image imgMap;
    if (!imgMap.loadFromFile(filename)) {
        error = true;
        return;
    }
    size = imgMap.getSize();

    // Copy raw pixel data from image into allocated buffer
    unsigned int bytesPerPixel = 4;
    terrain = new sf::Uint8[size.x * size.y * bytesPerPixel];
    memcpy((void*)terrain, (void*)imgMap.getPixelsPtr(), (size_t)(size.x * size.y * bytesPerPixel));

    // Find player start position
    for (unsigned int i=0; i< size.x * size.y * 4; i += 4) {
        if (terrain[i + 0] == 255 &&
            terrain[i + 1] == 127 &&
            terrain[i + 2] == 1) {

            // Save position
            playerSpawn.x = (float)((i / 4) % size.x);
            playerSpawn.y = (float)((i / 4) / size.x);

            // Make this pixel blank
            terrain[i + 0] = 0;
            terrain[i + 1] = 0;
            terrain[i + 2] = 0;
        }
    }

    // Create SFML texture from pixel data to be passed to shader for rendering
    txtTerrainData.create(size.x, size.y);
    txtTerrainData.update(terrain);

    // Load front-end textures
    if (!txtSolid.loadFromFile("data/solid.png")   ||
        !txtWater.loadFromFile("data/water.png")   ||
        !txtBouncy.loadFromFile("data/bouncy.png") ||
        !txtSlow.loadFromFile("data/slow.png")     ||
        !txtSticky.loadFromFile("data/sticky.png") ||
        !txtFinish.loadFromFile("data/finish.png")) {

        error = true;
        return;
    }

    txtSolid.setRepeated(true);
    txtWater.setRepeated(true);
    txtBouncy.setRepeated(true);
    txtSlow.setRepeated(true);
    txtSticky.setRepeated(true);
    txtFinish.setRepeated(true);

    sprTerrain.setTexture(txtSolid); // I think this needs to be set for size only?
    sprTerrain.setTextureRect(sf::IntRect(0, 0, (int)size.x, (int)size.y));

    // Load fragment shader
    if (!shdTerrain.loadFromFile("data/terrain.frag", sf::Shader::Fragment)) {
        error = true;
    }
}

EntityTerrain::~EntityTerrain()
{
    delete[] terrain;
}

sf::Vector2u EntityTerrain::get_size()
{
    return size;
}

bool EntityTerrain::loaded_successfuly()
{
    return !error;
}

bool EntityTerrain::get_solid(sf::Vector2f pos)
{
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

TerrainType EntityTerrain::get_pos(sf::Vector2f pos)
{
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

    if (terrain[base + 0] == 237 &&
        terrain[base + 1] == 28 &&
        terrain[base + 2] == 36) return T_WIN;

    return T_BLANK;
}

sf::Vector2f EntityTerrain::get_normal(sf::Vector2f pos)
{
    // TODO: Fix the memory rape

    std::vector<sf::Vector2f> positions;
    float sampleRadius = 4;
    for (float y=-sampleRadius; y<sampleRadius + 1; ++y) {
        for (float x=-sampleRadius; x<sampleRadius + 1; ++x) {
            sf::Vector2f p(pos.x + x, pos.y + y);
            if (pos_in_bounds(p) && (get_solid(sf::Vector2f(p.x, p.y)) || get_pos(p) == T_KILL))
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

bool EntityTerrain::intersects_with_circle(sf::Vector2f pos, float rad, sf::Vector2f *contact)
{
    const int divisions = 32; // Number of points around the circle to sample
    float deltaAngle = (2.f * (float)M_PI) / (float)divisions;

    // Determine if circle intersects (broad phase)
    bool intersects = false;
    for (int i = 0; i < divisions; ++i) {
        float angle = (float)i * deltaAngle;
        sf::Vector2f probe(pos.x + rad * cos(angle), pos.y + rad * sin(angle));

        if (get_solid(probe) || get_pos(probe) == T_KILL) {
            /*
             * Circle intersects with solid terrain.
             */
            intersects = true;
            break;
        }
    }

    if (!intersects)
        return false;

    /*
     * Find contact point (narrow phase)
     */

    // Get average of all intersection points on the edge of the circle
    sf::Vector2f pointsAverage;
    unsigned int numIntersections = 0;
    for (int i = 0; i < divisions; ++i) {
        float angle = (float)i * deltaAngle;
        sf::Vector2f probe(pos.x + rad * cos(angle), pos.y + rad * sin(angle));

        if (get_solid(probe) || get_pos(probe) == T_KILL) {
            pointsAverage += probe;
            ++numIntersections;
        }
    }

    if (numIntersections == divisions) {
        /*
         * Circle is completely inside terrain. A meaningful intersection point
         * cannot be found.
         */
        *contact = pos;
        return true;
    }

    assert(numIntersections != 0);
    pointsAverage /= (float)numIntersections;

    // Step towards center of circle to find edge of terrain
    sf::Vector2f probe = pointsAverage;
    sf::Vector2f toCenter = util::normalize(pos - pointsAverage);

    while (get_solid(probe) || get_pos(probe) == T_KILL) {
        probe += toCenter;
    }

    *contact = probe;
    return true;
}

void EntityTerrain::event(sf::Event &e)
{
}

void EntityTerrain::tick(std::vector<Entity*> &entities)
{
    world->set_camera_clamp(sf::Vector2f(0.f, 0.f), sf::Vector2f((float)size.x, (float)size.y));
}

void EntityTerrain::draw(sf::RenderWindow &window)
{
    if (render) {
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
        shdTerrain.setParameter("edgeColour", colour);
        shdTerrain.setParameter("renderEdges", (bool)options.edges == true ? 1.f : 0.f);

        sprTerrain.setPosition(-world->camera);
        window.draw(sprTerrain, &shdTerrain);
    }

    if (edit && ImGui::CollapsingHeader("Terrain")) {
        ImGui::TextDisabled(filename.c_str());
        ImGui::Checkbox("render", &render);
        ImGui::Checkbox("draw terrain normals", &drawTerrainNormals);
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
