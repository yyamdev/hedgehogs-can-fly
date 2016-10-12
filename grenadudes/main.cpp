#include <iostream>
#include <SFML/Graphics.hpp>
#include <memory>
#include "build_options.h"
#include "world.h"
#include "play_state.h"
#include "util.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), TITLE, sf::Style::Close);
    window.setFramerateLimit(60);

    World world(window);

    State::change_state(new StatePlay(&world));

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
            }

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::F1) {
                window.capture().saveToFile(util::to_string(time(NULL)) + ".png");
            }
            
            world.event(e);
            State::event_current(e);
        }

        world.tick();
        State::tick_current();
        
        window.clear(sf::Color(153, 217, 234));
        world.draw();
        State::draw_current(window);
        window.display();
    }

    State::free_memory();
    return 0;
}