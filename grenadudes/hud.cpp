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
    fnt.loadFromFile("data/VCR_OSD_MONO.ttf");
    aiState = AI_STATE_IDLE;
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

    // draw cooldown bars
    bar.setFillColor(sf::Color(0, 0, 255, 128));
    // player
    bar.setSize(sf::Vector2f(128.f * (cooldownPlayer / (float)DUDE_SHOOT_COOLDOWN), 16.f));
    bar.setPosition(sf::Vector2f(10.f, 42.f));
    window.draw(bar);
    // ai
    bar.setSize(sf::Vector2f(128.f * (cooldownAi / (float)DUDE_SHOOT_COOLDOWN), 16.f));
    bar.setPosition(sf::Vector2f((float)WINDOW_WIDTH - 128.f - 10.f, 42.f));
    window.draw(bar);

    // ai state
    sf::Text txt;
    txt.setFont(fnt);
    txt.setPosition(sf::Vector2f(256.f, 64.f));
    txt.setColor(sf::Color::Black);
    if (aiState == AI_STATE_IDLE) txt.setString("idle");
    if (aiState == AI_STATE_ATTACK) txt.setString("attack");
    if (aiState == AI_STATE_SEEK_PLAYER) txt.setString("seek player");
    if (aiState == AI_STATE_FLEE) txt.setString("flee");
    window.draw(txt);

    // wind
    draw_vector(sf::Vector2f(WINDOW_WIDTH / 2.f, 64.f), currentWind, util::len(currentWind) * 600.f, sf::Color::Blue, window);
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
    if (event == EVENT_DUDE_COOLDOWN_CHANGE) {
        EntityDude *dude = (EntityDude*)data;
        if (dude->get_number() == PLAYER_NUMBER)
            cooldownPlayer = util::clamp(dude->get_cooldown_time().asSeconds(), 0.f, (float)DUDE_SHOOT_COOLDOWN);
        else if (dude->get_number() == AI_NUMBER)
            cooldownAi = util::clamp(dude->get_cooldown_time().asSeconds(), 0.f, (float)DUDE_SHOOT_COOLDOWN);
    }
    if (event == EVENT_AI_STATE_CHANGE) {
        aiState = *((AiState*)data);
    }
    if (event == EVENT_WIND_CHANGE) {
        currentWind = *((sf::Vector2f*)data);
    }
}