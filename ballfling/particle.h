#pragma once

// particle system.

#include <SFML/Graphics.hpp>

struct Particle {
    Particle();
    Particle(sf::Vector2f position, sf::Color colour, sf::Vector2f velocity);
    Particle(sf::Vector2f position, sf::Color colour, sf::Vector2f velocity, sf::Vector2f size);

    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f size;
    sf::Color colour;
    bool active;
};

#define PARTICLE_NUM 512

class ParticleSystem {
public:
    ParticleSystem();
    void tick(sf::Vector2f camera);
    void draw(sf::RenderWindow &window, sf::Vector2f camera);
    void add_particle(Particle part);
private:
    int index; // index of next particle to be created
    Particle parts[PARTICLE_NUM];
    sf::Vector2f gravity;
};
extern ParticleSystem particleSystem;