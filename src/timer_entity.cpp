#include <iostream>
#include <sstream>
#include <iomanip>
#include <SFML/Graphics.hpp>
#include "timer_entity.h"
#include "shared_res.h"
#include "events.h"
#include "util.h"
#include "imgui.h"
#include "debug_draw.h"

std::string EntityTimer::get_formatted_time_str(unsigned int millis)
{
    unsigned int seconds = (millis / 1000);
    unsigned int minutes = seconds / 60;
    millis = millis % 1000;
    seconds = seconds % 60;
    std::stringstream format;
    format << std::setfill('0') << minutes << ".";
    format << std::setw(2) << seconds << ".";
    format << std::setw(3) << millis;

    return format.str();
}

EntityTimer::EntityTimer()
{
    timer.restart();
    timerPaused.restart();
    tag = "timer";
}

void EntityTimer::event(sf::Event &e) {}

void EntityTimer::tick(std::vector<Entity*> &entities) {}

void EntityTimer::draw(sf::RenderWindow &window)
{
    if (edit && ImGui::CollapsingHeader("Timer")) {
        if (ImGui::Button("toggle")) {
            enable = !enable;
        }
    }

    if (!enable) return;

    window.draw(sf::Text(get_formatted_time_str(get_time()), fntUi));
}

void EntityTimer::on_notify(Event event, void *data)
{
    if (event == EVENT_BALL_FLING && !start) {
        start = true;
        timer.restart();
    }

    if (event == EVENT_LEVEL_COMPLETE) {
        millisecondsFinish = get_time();
        finish = true;
    }

    if (event == EVENT_PLAY_RESUME) {
        paused = false;
        millisecondsPaused += timerPaused.getElapsedTime().asMilliseconds();
    }

    if (event == EVENT_PLAY_PAUSE) {
        millisecondsBeforePause = get_time();
        paused = true;
        timerPaused.restart();
    }
}

unsigned int EntityTimer::get_time()
{
    if (finish) return millisecondsFinish;

    if (!start) return 0;

    if (paused)
        return millisecondsBeforePause;
    else
        return timer.getElapsedTime().asMilliseconds() - millisecondsPaused;
}
