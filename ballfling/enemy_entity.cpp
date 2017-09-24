#include "enemy_entity.h"
#include <SFML/Graphics.hpp>
#include "world.h"
#include "util.h"
#include <iostream>
#include "particle.h"
#include <math.h>

EntityEnemy::EntityEnemy()
{
    std::cout << "Don't use EntityEnemy default contructor!!\n";
}

EntityEnemy::EntityEnemy(sf::Vector2f pos, sf::Vector2f *playerPos)
{
    position = pos;
    player = playerPos;
    state = ENEMY_STATE_INACTIVE;
    static float angle = 0.f; // radians
    static float len = 0.f;
    pulseTimer.restart();
    colour = sf::Color(172, 50, 50);
}

void EntityEnemy::event(sf::Event &e)
{
}

void EntityEnemy::tick(std::vector<Entity*> &entities)
{
    if (state == ENEMY_STATE_INACTIVE) {
        position = *player;
    }
    
    if (state == ENEMY_STATE_WAITING) {
        for (int i = 0; i < 10; ++i) {
            sf::Vector2f toEdge = len * sf::Vector2f(cos(angle), sin(angle));
            add_particle(position + toEdge, util::normalize(toEdge) * 4.f, sf::Vector2f(), sf::Color(217, 87, 99), 30);
            angle += .04f;
            len += .05f;
        }
        
        if (waitingTimer.getElapsedTime().asSeconds() >= 1.f)
            state = ENEMY_STATE_ACTIVE;
    }
    
    if (state != ENEMY_STATE_ACTIVE)
        return;
    
    sf::Vector2f delta = *player - position;

    if (util::len(delta) == 0.f) return;
    
    sf::Vector2f direction = util::normalize(delta);

    sf::Vector2f speed;

    speed.x = util::len(delta) / 300.f * 1.f;
    speed.x = util::clamp(speed.x, 0.1f, 1.f);

    speed.y = util::len(delta) / 300.f * .7f;
    speed.y = util::clamp(speed.y, 0.1f, .7f);

    position += sf::Vector2f(direction.x * speed.x, direction.y * speed.y);

    if (pulseTimer.getElapsedTime().asSeconds() > 2.f) {
        float maxI = 1024;
        for (float i = 0; i < maxI; ++i) {
            float angle = (2.f * 3.14f) * (i / maxI);
            sf::Vector2f vel = sf::Vector2f(cos(angle), sin(angle)) * 4.f;
            add_particle(position, vel, sf::Vector2f(), colour, 180);
        }
        pulseTimer.restart();
    }
}

void EntityEnemy::draw(sf::RenderWindow &window)
{
    if (state != ENEMY_STATE_ACTIVE)
        return;
    
    float size = 32.f;
    sf::CircleShape circle;
    circle.setPosition(position - world->camera);
    circle.setOrigin(sf::Vector2f(size, size));
    circle.setRadius(size);
    circle.setFillColor(colour);
    window.draw(circle);
}

void EntityEnemy::on_notify(Event event, void *data)
{
    if (event == EVENT_BALL_HIT_TRIGGER && state == ENEMY_STATE_INACTIVE) {
        state = ENEMY_STATE_WAITING;
        waitingTimer.restart();
    }
}
