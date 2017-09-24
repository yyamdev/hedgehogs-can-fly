#include "timer_entity.h"
#include <SFML/Graphics.hpp>
#include "shared_res.h"
#include "events.h"
#include "util.h"
#include <iostream>
#include <sstream>
#include <iomanip>

EntityTimer::EntityTimer() {
    millisecondsPaused = 0;
    millisecondsBeforePause = 0;
    millisecondsFinish = 0;
    timer.restart();
    timerPaused.restart();
    p = true;
    start = false;
    finish = false;
}

void EntityTimer::event(sf::Event &e) {
}

void EntityTimer::tick(std::vector<Entity*> &entities) {
}

void EntityTimer::draw(sf::RenderWindow &window) {
    return; // disabled
    
    unsigned int millis  = get_time();
    unsigned int seconds = (millis / 1000);
    unsigned int minutes = seconds / 60;
    millis = millis % 1000;
    seconds = seconds % 60;
    std::stringstream format;
    format << std::setfill('0') << minutes << ".";
    format << std::setw(2) << seconds << ".";
    format << std::setw(3) << millis;
    sf::Text timerDraw(format.str(), fntUi);
    window.draw(timerDraw);
}

void EntityTimer::on_notify(Event event, void *data) {
    if (event == EVENT_BALL_FLING && !start) {
        start = true;
        timer.restart();
    }

    if (event == EVENT_BALL_HIT_FINISH) {
        millisecondsFinish = get_time();
        finish = true;
    }

    if (event == EVENT_PLAY_RESUME) {
        p = false;
        millisecondsPaused += timerPaused.getElapsedTime().asMilliseconds();
    }

    if (event == EVENT_PLAY_PAUSE) {
        millisecondsBeforePause = get_time();
        p = true;
        timerPaused.restart();
    }
}

unsigned int EntityTimer::get_time() {
    if (finish) return millisecondsFinish;
    
    if (!start) return 0;
    
    if (p)
        return millisecondsBeforePause;
    else
        return timer.getElapsedTime().asMilliseconds() - millisecondsPaused;
}
