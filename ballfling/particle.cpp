#include "particle.h"
#include "build_options.h"

ParticleSystem particleSystem;

Particle::Particle() {
    Particle(sf::Vector2f(0.f, 0.f), sf::Color::White, sf::Vector2f(0.f, 0.f));
}

Particle::Particle(sf::Vector2f position, sf::Color colour, sf::Vector2f velocity) {
    Particle(sf::Vector2f(0.f, 0.f), sf::Color::White, sf::Vector2f(0.f, 0.f), sf::Vector2f(10.f, 10.f));
}

Particle::Particle(sf::Vector2f position, sf::Color colour, sf::Vector2f velocity, sf::Vector2f size) {
    this->position = position;
    this->colour = colour;
    this->velocity = velocity;
    this->size = size;
    active = false;
}

ParticleSystem::ParticleSystem() {
    index = 0;
    gravity = sf::Vector2f(0.f, GRAVITY);
}

void ParticleSystem::draw(sf::RenderWindow &window, sf::Vector2f camera) {
    for (unsigned int i = 0; i < PARTICLE_NUM; ++i) {
        Particle &p = parts[i];
        if (!p.active) continue;
        sf::RectangleShape pixel(p.size);
        pixel.setOrigin(pixel.getSize() / 2.f);
        pixel.setPosition(p.position - camera);
        pixel.setFillColor(p.colour);
        window.draw(pixel);
    }
}

void ParticleSystem::add_particle(Particle part) {
    parts[index] = part;
    parts[index].active = true;
    index = (index + 1) % PARTICLE_NUM;
}

void ParticleSystem::tick(sf::Vector2f camera) {
    for (unsigned int i = 0; i < PARTICLE_NUM; ++i) {
        Particle &p = parts[i];
        if (!p.active) continue;
        p.position += p.velocity;
        p.velocity += gravity;
        if (p.position.x < camera.x || p.position.x > camera.x + WINDOW_WIDTH || p.position.y < camera.y || p.position.y > camera.y + WINDOW_HEIGHT)
            p.active = false;
    }
}