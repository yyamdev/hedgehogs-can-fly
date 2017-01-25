#include "hud.h"
#include "subject.h"
#include <SFML/Graphics.hpp>
#include "debug_draw.h"
#include "util.h"
#include "build_options.h"

Hud::Hud() {
    Subject::add_observer(this); // register for events
    dragging = false;
}

void Hud::draw(sf::RenderWindow &window) {
    if (dragging) { // draw mouse drag for grenade throw
        sf::Vector2i mouseI = sf::Mouse::getPosition(window);
        sf::Vector2f mouse;
        mouse.x = (float)mouseI.x;
        mouse.y = (float)mouseI.y;
        draw_vector(mouseDragStart, mouse - mouseDragStart, util::len(mouse - mouseDragStart), sf::Color::Red, window);
    }

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
}