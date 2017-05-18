#include "particle.h"
#include "build_options.h"

ParticleSystem particleSystem;

ParticleSystem::ParticleSystem() {
    index = 0;
    gravity = sf::Vector2f(0.f, GRAVITY);
}

void ParticleSystem::draw(sf::RenderWindow &window, sf::Vector2f camera) {
    for (unsigned int i = 0; i < PARTICLE_NUM; ++i) {
        Particle &p = parts[i];
        if (!p.active) continue;
        shape.setSize(p.size);
        shape.setOrigin(shape.getSize() / 2.f);
        shape.setPosition(p.position - camera);
        shape.setFillColor(p.colour);
        window.draw(shape);
    }
}

void ParticleSystem::add_particle(Particle part) {
    parts[index] = part;
    parts[index].active = true;
    index = (index + 1) % PARTICLE_NUM;
}

void ParticleSystem::tick(sf::Vector2f camera) {
    for (unsigned int i = 0; i < PARTICLE_NUM; ++i) {
        if (!parts[i].active) continue;
        parts[i].position += parts[i].velocity;
        parts[i].velocity += parts[i].gravity;
        if (--parts[i].lifetime < 0)
            parts[i].active = false;
        if (parts[i].lifetime < 255)
            parts[i].colour.a = parts[i].lifetime;
        //if (p.position.x < camera.x || p.position.x > camera.x + WINDOW_WIDTH || p.position.y < camera.y || p.position.y > camera.y + WINDOW_HEIGHT)
            //p.active = false;
    }
}