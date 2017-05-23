#pragma once

// Fast particle system.

#include <SFML/Graphics.hpp>

struct Particle {
    float position[2];
    float velocityVec[2];
    float accelerationVec[2];
    int lifetime;
    bool active;
    union {
        sf::Uint8 colour[4];
        sf::Uint32 colourInt;
    };
    int _startLifetime;
};

#define PARTICLE_NUM 4096
extern Particle particles[PARTICLE_NUM];
extern sf::Vertex particleVertex[PARTICLE_NUM];
extern int particleIndex;

void particles_tick(sf::Vector2f camera);
void particles_draw(sf::RenderWindow &window, sf::Vector2f camera);
void add_particle(Particle *part);
void add_particle(sf::Vector2f position, sf::Vector2f velocity, sf::Vector2f acceleration, sf::Color colour, int lifetime);