#pragma once

/*
 * Level timer.
 */

#include <string>
#include <SFML/System.hpp>
#include "entity.h"
#include "subject.h"
#include "observer.h"

class EntityTimer : public Entity, public Observer {
public:
    EntityTimer();

    void event(sf::Event &e);
    void tick(std::vector<Entity*> &entities);
    void draw(sf::RenderWindow &window);

    void on_notify(Event event, void *data);

    unsigned int get_time();
    static std::string get_formatted_time_str(unsigned int millis);
private:
    sf::Clock timer;
    sf::Clock timerPaused;
    unsigned int millisecondsBeforePause = 0;
    unsigned int millisecondsPaused = 0;
    unsigned int millisecondsFinish = 0;
    bool paused = true;
    bool start = false;
    bool finish = false;
    bool enable = true;
};
