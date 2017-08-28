#pragma once

// Level timer

#include "entity.h"
#include "subject.h"
#include "observer.h"
#include <SFML/System.hpp>

class EntityTimer : public Entity, public Observer {
public:
    EntityTimer();

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

    void on_notify(Event event, void *data);

    unsigned int get_time();
private:
    sf::Clock timer;
    sf::Clock timerPaused;
    unsigned int millisecondsBeforePause;
    unsigned int millisecondsPaused;
    unsigned int millisecondsFinish;
    bool p;
    bool start;
    bool finish;
};
