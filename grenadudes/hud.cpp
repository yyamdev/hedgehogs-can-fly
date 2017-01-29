#include "hud.h"
#include "subject.h"
#include <SFML/Graphics.hpp>
#include "debug_draw.h"
#include "util.h"
#include "build_options.h"

Hud::Hud() {
    Subject::add_observer(this); // register for events
    dragging = false;
    moving = true;
    txtArrow.loadFromFile("data/arrow.png");
    txtCursorDrag.loadFromFile("data/cursor_drag.png");
    txtCursorStop.loadFromFile("data/cursor_stop.png");
}

void Hud::draw(sf::RenderWindow &window, sf::Vector2f camera) {
    sf::Vector2i mouseI = sf::Mouse::getPosition(window);
    sf::Vector2f mouse;
    mouse.x = (float)mouseI.x;
    mouse.y = (float)mouseI.y;

    // draw mouse drag visual indicators
    if (dragging) {
        sf::Vector2f dir = mouseDragStart -  mouse;
        float mag = util::len(dir) / 15.f;
        float scale = fmin(mag / BALL_MAX_SPEED, 1.f);
        float ang = atan2f(dir.y, dir.x) * (180.f / PI_F);
        // draw red line
        draw_vector(mouse, dir, util::len(mouse - mouseDragStart), sf::Color::Red, window);
        // draw arrow
        sf::Sprite sprArrow(txtArrow);
        sprArrow.setOrigin(sf::Vector2f(0.f, (float)txtArrow.getSize().y / 2.f));
        sprArrow.setPosition(ballRestPos - camera);
        sprArrow.setScale(sf::Vector2f(scale, 1.f));
        sprArrow.setRotation(ang);
        sprArrow.setColor(sf::Color(255, 255, 255, 128));
        window.draw(sprArrow);
    }

    // wind
    draw_vector(sf::Vector2f(WINDOW_WIDTH / 2.f, 64.f), currentWind, util::len(currentWind) * 600.f, sf::Color::Blue, window);

    // draw cursors
    sf::Sprite sprCursor;
    if (moving) sprCursor.setTexture(txtCursorStop);
    else        sprCursor.setTexture(txtCursorDrag);
    sprCursor.setOrigin(sf::Vector2f(13.f, 13.f));
    sprCursor.setPosition(mouse);
    window.draw(sprCursor);
}

void Hud::on_notify(Event event, void *data) {
    if (event == EVENT_PLAYER_START_DRAG) {
        mouseDragStart = *((sf::Vector2f*)data);
        dragging = true;
    }
    if (event == EVENT_PLAYER_END_DRAG) {
        dragging = false;
    }
    if (event == EVENT_BALL_REST_POS) {
        ballRestPos = *((sf::Vector2f*)data);
        moving = false;
    }
    if (event == EVENT_BALL_START_MOVING) {
        moving = true;
    }
}