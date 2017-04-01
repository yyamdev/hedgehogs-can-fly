#include "particle.h"
#include "build_options.h"

ParticleSystem particleSystem;

Particle::Particle() {
    Particle(sf::Vector2f(0.f, 0.f), sf::Color::White, sf::Vector2f(0.f, 0.f));
}

Particle::Particle(sf::Vector2f position, sf::Color colour, sf::Vector2f velocity) {
    this->position = position;
    this->colour = colour;
    this->velocity = velocity;
    active = false;
}

ParticleSystem::ParticleSystem() {
    index = 0;
    gravity = sf::Vector2f(0.f, GRAVITY);
}

void ParticleSystem::draw(sf::RenderWindow &window) {
    for (unsigned int i = 0; i < PARTICLE_NUM; ++i) {
        Particle &p = parts[i];
        if (!p.active) continue;
        sf::RectangleShape pixel(sf::Vector2f(1.f, 1.f));
        pixel.setPosition(p.position);
        pixel.setFillColor(p.colour);
        window.draw(pixel);
    }
}

void ParticleSystem::add_particle(Particle part) {
    parts[index] = part;
    parts[index].active = true;
    index = (index + 1) % PARTICLE_NUM;
}

void ParticleSystem::tick() {
    for (unsigned int i = 0; i < PARTICLE_NUM; ++i) {
        Particle &p = parts[i];
        if (!p.active) continue;
        p.position += p.velocity;
        p.velocity += gravity;
    }
}