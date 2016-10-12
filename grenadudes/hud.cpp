#include "hud.h"
#include "subject.h"
#include <SFML/Graphics.hpp>
#include "debug_draw.h"
#include "util.h"
#include "build_options.h"
#include "dude_entity.h"

Hud::Hud() {
    Subject::add_observer(this); // register for events
    dragging = false;
    hpPlayer.hp = 0;
    hpPlayer.maxHp = 1;
    hpAi.hp = 0;
    hpAi.maxHp = 1;
}

void Hud::draw(sf::RenderWindow &window) {
    if (dragging) { // draw mouse drag for grenade throw
        sf::Vector2i mouseI = sf::Mouse::getPosition(window);
        sf::Vector2f mouse;
        mouse.x = (float)mouseI.x;
        mouse.y = (float)mouseI.y;
        draw_vector(mouseDragStart, mouse - mouseDragStart, util::len(mouse - mouseDragStart), sf::Color::Red, window);
    }

    // draw hp bars
    sf::RectangleShape bar;
    bar.setFillColor(sf::Color::Green);
    // player
    bar.setSize(sf::Vector2f(128.f * ((float)hpPlayer.hp / (float)hpPlayer.maxHp), 32.f));
    bar.setPosition(sf::Vector2f(10.f, 10.f));
    window.draw(bar);
    // ai
    bar.setSize(sf::Vector2f(128.f * ((float)hpAi.hp / (float)hpAi.maxHp), 32.f));
    bar.setPosition(sf::Vector2f((float)WINDOW_WIDTH - 128.f - 10.f, 10.f));
    window.draw(bar);
}

void Hud::on_notify(Event event, void *data) {
    if (event == EVENT_PLAYER_START_DRAG) {
        mouseDragStart = *((sf::Vector2f*)data);
        dragging = true;
    }
    if (event == EVENT_PLAYER_END_DRAG) {
        dragging = false;
    }
    if (event == EVENT_DUDE_HP_CHANGE) {
        EntityDude *dude = (EntityDude*)data;
        if (dude->get_number() == PLAYER_NUMBER) {
            hpPlayer.hp = dude->get_hp();
            hpPlayer.maxHp = dude->get_max_hp();
        }
        else if (dude->get_number() == AI_NUMBER) {
            hpAi.hp = dude->get_hp();
            hpAi.maxHp = dude->get_max_hp();
        }
    }
}