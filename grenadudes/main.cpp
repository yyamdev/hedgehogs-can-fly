#include <iostream>
#include <SFML/Graphics.hpp>
#include <memory>
#include "build_options.h"
#include "world.h"
#include "play_state.h"
#include "util.h"

void print_debug_controls();

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), TITLE, sf::Style::Close);
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);

    std::cout << "OpenGL version " << window.getSettings().majorVersion << "." << window.getSettings().minorVersion << std::endl;

    World world(window);

    State::change_state(new StatePlay(&world, "data/map.png"));

    print_debug_controls();

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
            }

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::F1)
                window.capture().saveToFile(util::to_string(time(NULL)) + ".png");

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::R)
                State::change_state(new StatePlay(&world, "data/map.png"));

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::H)
                print_debug_controls();

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::C)
                system("cls");
            
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

void print_debug_controls() {
    std::cout << "debug controls:\n";
    std::cout << "B - toggle pause\n";
    std::cout << "P - toggle ai\n";
    std::cout << "N - toggle show pathfinding nodes\n";
    std::cout << "G - toggle show terrain grix approx.\n";
    std::cout << "middle mouse - ai navigate\n";
    std::cout << "F1 - screenshot\n";
    std::cout << "R - restart game\n";
    std::cout << "H - print this message\n";
    std::cout << "C - clear console\n";
}