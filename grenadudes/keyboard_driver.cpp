#include "keyboard_driver.h"
#include <iostream>
#include "util.h"

KeyboardDriver::KeyboardDriver(EntityDude *dude) : DudeDriver(dude) {
    dragging = false;
    dragStart = sf::Vector2f();
}

void KeyboardDriver::event(sf::Event &e) {
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        if (!dragging) {
            dragging = true;
            dragStart.x = (float)e.mouseButton.x;
            dragStart.y = (float)e.mouseButton.y;
            notify(EVENT_PLAYER_START_DRAG, (void*)(&dragStart));
        }
    }
    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        if (dragging) {
            dragging = false;
            sf::Vector2f mouse;
            mouse.x = (float)e.mouseButton.x;
            mouse.y = (float)e.mouseButton.y;
            dude->throw_grenade(dragStart - mouse, util::len(dragStart - mouse) / 15.f);
            notify(EVENT_PLAYER_END_DRAG, NULL);
        }
    }
}

void KeyboardDriver::tick(std::vector<Entity*> &entities) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        dude->move(EntityDude::DIRECTION_LEFT);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        dude->move(EntityDude::DIRECTION_RIGHT);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        dude->jump();
}