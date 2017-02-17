#pragma once

// tnt box that can explode

#include "entity.h"
#include <SFML/Graphics.hpp>

class EntityTerrain;

class EntityTnt : public Entity {
public:
    EntityTnt();
    EntityTnt(sf::Vector2f pos);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

    bool intersects_with_circle(sf::Vector2f pos, float rad);
    sf::Vector2f get_normal(sf::Vector2f pos, sf::Vector2f vel);
    void touch();
private:
    EntityTerrain *terrain;
    bool touched;
    int pulses;
    sf::Clock clkPulse; // for graphical and sound effects
    bool pulse;

    static sf::Texture txt;
    static bool textureLoaded;

    bool intersecting_terrain();
};

class EntityExplosionGfx : public Entity {
public:
    EntityExplosionGfx();
    EntityExplosionGfx(sf::Vector2f pos, float rad);

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);
private:
    sf::Clock life;
    float rad;
};