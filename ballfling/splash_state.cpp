#include "splash_state.h"
#include "cursor.h"
#include "build_options.h"
#include <iostream>
#include "menu_state.h"

StateSplash::StateSplash(World *world) : State(world) {
    set_cursor_visible(false);
    state = 0;
    alpha = 0.f;
    txtSplash.loadFromFile("data/splash.png");
    sprSplash.setTexture(txtSplash);
    sprSplash.setPosition(WINDOW_WIDTH / 2.f - txtSplash.getSize().x / 2.f,
                          WINDOW_HEIGHT / 2.f - txtSplash.getSize().y / 2.f);
    mouseEventCount = 0;
}

void StateSplash::on_event(sf::Event &event) {
    if (event.type == sf::Event::MouseButtonPressed)
        State::change_state(new StateMenu(world));
}

void StateSplash::on_tick() {
    switch (state) {
    case 0: // fade in
        alpha += .01f;
        if (alpha > 1.f) {
            alpha = 1.f;
            state = 1;
            timer.restart();
        }
        break;
    case 1: // hold
        if (timer.getElapsedTime().asSeconds() > 2.f) {
            state = 2;
            //notify(EVENT_ENTER_SPLASH, NULL);
        }
        break;
    case 2: // fade out
        alpha -= .06f;
        if (alpha < 0.f) {
            State::change_state(new StateMenu(world));
        }
        break;
    }
}

void StateSplash::on_draw(sf::RenderWindow &window) {
    sf::Uint8 ualpha = (unsigned char)(alpha * 255.f);
    sprSplash.setColor(sf::Color(255, 255, 255, ualpha));
    window.draw(sprSplash);
}

void StateSplash::on_draw_ui(sf::RenderWindow &window) {
}

void StateSplash::on_gain_focus() {
    notify(EVENT_ENTER_SPLASH, NULL);
}

void StateSplash::on_lose_focus() {
  notify(EVENT_EXIT_SPLASH, NULL);
}
