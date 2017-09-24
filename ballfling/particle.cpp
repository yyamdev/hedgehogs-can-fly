#include "particle.h"
#include "build_options.h"
#include "debug_draw.h"
#include "imgui.h"
#include <algorithm>
#include "util.h"

static Particle particles[PARTICLE_NUM];
static sf::Vertex particleVertex[PARTICLE_NUM];
static int particleIndex = 0;

void particles_tick(sf::Vector2f camera) {
    for (int i = 0; i < PARTICLE_NUM; ++i) {
        if (!particles[i].active) continue;

        // Move & accelerate.
        particles[i].position[0] += particles[i].velocityVec[0];
        particles[i].position[1] += particles[i].velocityVec[1];
        particles[i].velocityVec[0] += particles[i].accelerationVec[0];
        particles[i].velocityVec[1] += particles[i].accelerationVec[1];
        
        // Adjust alpha component of colour with lifetime.
        particles[i].colour[3] = (sf::Uint8)(255 * particles[i].lifetime / particles[i]._startLifetime);

        // Kill due to lifetime.
        if (particles[i].lifetime-- < 0) {
            particles[i].active = false;
        }

        // Kill due to offscreen
        /*
        else if (particles[i].position[0] < camera.x ||
            particles[i].position[0] > camera.x + WINDOW_WIDTH ||
            particles[i].position[1] < camera.y ||
            particles[i].position[1] > camera.y + WINDOW_HEIGHT) {
            particles[i].active = false;
        }
        */

        // React to actors.
        // This seems quite slow
        /*
        float actorToParticleLen  = util::len_squared(sf::Vector2f(particles[i].position[0], particles[i].position[1]) - actorPosition);
        float cutoff = 1024.f;
        float strength = fmax(0.f, cutoff - actorToParticleLen) / cutoff;
        strength *= .5f;
        particles[i].velocityVec[0] += strength * actorVelocity.x;
        particles[i].velocityVec[1] += strength * actorVelocity.y;
        */
    }
}

void particles_draw(sf::RenderWindow &window, sf::Vector2f camera) {
    int vertexIndex = 0;
    for (int i = 0; i < PARTICLE_NUM; ++i) {
        if (particles[i].active) {
            particleVertex[vertexIndex].position.x = particles[i].position[0] - camera.x;
            particleVertex[vertexIndex].position.y = particles[i].position[1] - camera.y;
            particleVertex[vertexIndex].color = sf::Color(
                particles[i].colour[0],
                particles[i].colour[1],
                particles[i].colour[2],
                particles[i].colour[3]);
            vertexIndex++;
        }
    }

    window.draw(particleVertex, vertexIndex, sf::PrimitiveType::Points);

    if (edit && ImGui::CollapsingHeader("particles")) {
        ImGui::Text("Particles on screen: %i", vertexIndex);
    }
}

void add_particle(Particle *part) {
    particles[particleIndex] = *part;
    particles[particleIndex].active = true;
    particles[particleIndex]._startLifetime = particles[particleIndex].lifetime;
    particleIndex = (particleIndex + 1) % PARTICLE_NUM;
}

void add_particle(sf::Vector2f position, sf::Vector2f velocity, sf::Vector2f acceleration, sf::Color colour, int lifetime) {
    particles[particleIndex].position[0] = position.x;
    particles[particleIndex].position[1] = position.y;
    particles[particleIndex].velocityVec[0] = velocity.x;
    particles[particleIndex].velocityVec[1] = velocity.y;
    particles[particleIndex].accelerationVec[0] = acceleration.x;
    particles[particleIndex].accelerationVec[1] = acceleration.y;
    particles[particleIndex].colour[0] = colour.r;
    particles[particleIndex].colour[1] = colour.g;
    particles[particleIndex].colour[2] = colour.b;
    particles[particleIndex].colour[3] = colour.a;
    particles[particleIndex].lifetime = lifetime;
    particles[particleIndex]._startLifetime = lifetime;
    particles[particleIndex].active = true;
    particleIndex = (particleIndex + 1) % PARTICLE_NUM;
}
