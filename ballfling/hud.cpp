#include "hud.h"
#include "subject.h"
#include <SFML/Graphics.hpp>
#include "debug_draw.h"
#include "util.h"
#include "build_options.h"
#include "ball_entity.h"

Hud::Hud() {
    Subject::add_observer(this); // register for events
    dragging = false;
    canFling = canNudge = false;
    txtArrow.loadFromFile("data/arrow.png");
    txtCursorDrag.loadFromFile("data/cursor_drag.png");
    txtCursorStop.loadFromFile("data/cursor_stop.png");
    txtCursorNudge.loadFromFile("data/cursor_nudge.png");
    moveCount = 0;
    fntCounter.loadFromFile("data/VCR_OSD_MONO.ttf");
    txtInstructionDrag.loadFromFile("data/instruction1.png");
    txtInstructionSpace.loadFromFile("data/instruction2.png");
    sprInstructionDrag.setTexture(txtInstructionDrag);
    sprInstructionSpace.setTexture(txtInstructionSpace);
    sprInstructionDrag.setOrigin(sf::Vector2f(txtInstructionDrag.getSize().x / 2.f, txtInstructionDrag.getSize().y / 2.f));
    sprInstructionSpace.setOrigin(sf::Vector2f(txtInstructionSpace.getSize().x / 2.f, txtInstructionSpace.getSize().y / 2.f));
    sprInstructionSpace.setScale(sf::Vector2f(0.7f, 0.7f));
}

void Hud::draw(sf::RenderWindow &window, sf::Vector2f camera) {
    sf::Vector2i mouseI = sf::Mouse::getPosition(window);
    sf::Vector2f mouse;
    mouse.x = (float)mouseI.x;
    mouse.y = (float)mouseI.y;

    // draw instructions
    if (SHOW_INITIAL_INSTRUCTIONS) {
        sprInstructionDrag.setPosition(sf::Vector2f(200.f - camera.x, 2200.f - camera.y));
        sprInstructionSpace.setPosition(sf::Vector2f(950.f - camera.x, 2260.f - camera.y));
        window.draw(sprInstructionDrag);
        window.draw(sprInstructionSpace);
    }

    // draw mouse drag visual indicators
    if (dragging && canFling) {
        sf::Vector2f dir = mouseDragStart -  mouse;
        float mag = util::len(dir) / 15.f;
        float scale = fmin(mag / BALL_MAX_LAUNCH_SPEED, 1.f);
        float ang = atan2f(dir.y, dir.x) * (180.f / PI_F);
        // draw red line
        draw_vector(mouseDragStart, -dir, fmin(util::len(mouseDragStart - mouse), BALL_MAX_LAUNCH_SPEED * 15.f), sf::Color::Red, window);
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
    if (canNudge) sprCursor.setTexture(txtCursorNudge);
    if (canFling) sprCursor.setTexture(txtCursorDrag);
    if (!canNudge && !canFling) sprCursor.setTexture(txtCursorStop);
    sprCursor.setOrigin(sf::Vector2f(13.f, 13.f));
    sprCursor.setPosition(mouse);
    window.draw(sprCursor);

    sf::Text txtCounter(util::to_string(moveCount), fntCounter);
    txtCounter.setPosition(sf::Vector2f(20.f, 10.f));
    txtCounter.setColor(sf::Color::Black);
    window.draw(txtCounter);
}

void Hud::on_notify(Event event, void *data) {
    if (event == EVENT_PLAYER_START_DRAG) {
        mouseDragStart = *((sf::Vector2f*)data);
        dragging = true;
    }
    if (event == EVENT_PLAYER_END_DRAG) {
        dragging = false;
        moveCount += 1;
    }
    if (event == EVENT_BALL_REST_POS || event == EVENT_BALL_MOVE) {
        ballRestPos = *((sf::Vector2f*)data);
    }
    if (event == EVENT_BALL_CHANGE_CAN_FLING) {
        canFling = *((bool*)data);
    }
    if (event == EVENT_BALL_CHANGE_CAN_NUDGE) {
        canNudge = *((bool*)data);
    }
    if (event == EVENT_PLAYER_END_DRAG)
        sprInstructionDrag.setColor(sf::Color(255,255,255,0));
    if (event == EVENT_PRESS_SPACE)
        sprInstructionSpace.setColor(sf::Color(255,255,255,0));
}