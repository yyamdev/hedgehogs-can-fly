#include "enemy_entity.h"
#include <SFML/Graphics.hpp>
#include "world.h"
#include "util.h"
#include <iostream>

EntityEnemy::EntityEnemy()
{
    std::cout << "Don't use EntityEnemy default contructor!!\n";
}

EntityEnemy::EntityEnemy(sf::Vector2f pos, sf::Vector2f *playerPos)
{
    position = pos;
    player = playerPos;
    state = ENEMY_STATE_INACTIVE;
}

void EntityEnemy::event(sf::Event &e)
{
}

void EntityEnemy::tick(std::vector<Entity*> &entities)
{
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
}

void EntityEnemy::draw(sf::RenderWindow &window)
{
    if (state == ENEMY_STATE_WAITING &&
        waitingTimer.getElapsedTime().asSeconds() >= 1.f) {

        state = ENEMY_STATE_ACTIVE;
    }
    
    if (state != ENEMY_STATE_ACTIVE)
        return;
    
    float size = 32.f;
    sf::CircleShape circle;
    circle.setPosition(position - world->camera);
    circle.setOrigin(sf::Vector2f(size, size));
    circle.setRadius(size);
    window.draw(circle);
}

void EntityEnemy::on_notify(Event event, void *data)
{
    if (event == EVENT_BALL_FLING && state == ENEMY_STATE_INACTIVE) {
        state = ENEMY_STATE_WAITING;
        waitingTimer.restart();
    }
}
