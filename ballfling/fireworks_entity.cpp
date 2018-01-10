#include <iostream>
#include "fireworks_entity.h"
#include "particle.h"
#include "util.h"
#include "state.h"

EntityFireworks::EntityFireworks()
{
    EntityFireworks(sf::Vector2f(0.f, 0.f));
}

EntityFireworks::EntityFireworks(sf::Vector2f pos)
{
    position = pos;
    timer.restart();
}

void EntityFireworks::event(sf::Event &e)
{
}

void EntityFireworks::do_fireworks(sf::Vector2f position, int count)
{
    for (int i = 0; i < count; ++i)
    {
        add_particle(position,
                     sf::Vector2f(util::rnd(-2.f, 2.f), util::rnd(-3.f, -1.f)),
                     sf::Vector2f(0.f, 0.1f),
                     util::choose(sf::Color::Yellow, sf::Color::White),
                     360);
    }
    notify(EVENT_FIREWORK, NULL);
}

void EntityFireworks::tick(std::vector<Entity*> &entities)
{
    float time = timer.getElapsedTime().asSeconds();
    switch (count)
    {
    case 0:
        if (time > .5f)
        {
            do_fireworks(position + sf::Vector2f(0.f, -64.f), 30);
            ++count;
            timer.restart();
        }
        break;
    case 1:
        if (time > .25f)
        {
            do_fireworks(position + sf::Vector2f(-64.f, -16.f), 30);
            ++count;
            timer.restart();
        }
        break;
    case 2:
        if (time > .25f)
        {
            do_fireworks(position + sf::Vector2f(64.f, -32.f), 30);
            ++count;
            timer.restart();
        }
        break;
    case 3:
        if (time > .25f)
        {
            do_fireworks(position + sf::Vector2f(-48.f, -48.f), 30);
            ++count;
            timer.restart();
        }
        break;
    case 4:
        if (time > 2.f)
        {
            notify(EVENT_BALL_HIT_FINISH, NULL);
            ++count;
        }
        break;
    }
}

void EntityFireworks::draw(sf::RenderWindow &window)
{
}
