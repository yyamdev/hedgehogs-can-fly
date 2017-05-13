#include "hud.h"
#include "subject.h"
#include <SFML/Graphics.hpp>
#include "debug_draw.h"
#include "util.h"
#include "build_options.h"
#include "ball_entity.h"
#include "imgui.h"
#include "cursor.h"

Hud::Hud() {
    Subject::add_observer(this); // register for events
    dragging = false;
    canFling = canNudge = false;
    txtArrow.loadFromFile("data/arrow.png");
    txtBallNudge.loadFromFile("data/ball_nudge.png");
    moveCount = 0;
    fntCounter.loadFromFile("data/VCR_OSD_MONO.ttf");
    txtInstructionDrag.loadFromFile("data/instruction1.png");
    txtInstructionSpace.loadFromFile("data/instruction2.png");
    sprInstructionDrag.setTexture(txtInstructionDrag);
    sprInstructionSpace.setTexture(txtInstructionSpace);
    sprInstructionDrag.setOrigin(sf::Vector2f(txtInstructionDrag.getSize().x / 2.f, txtInstructionDrag.getSize().y / 2.f));
    sprInstructionSpace.setOrigin(sf::Vector2f(txtInstructionSpace.getSize().x / 2.f, txtInstructionSpace.getSize().y / 2.f));
    sprInstructionSpace.setScale(sf::Vector2f(0.7f, 0.7f));
    drawArrowOnBall = true;
    drawMoveCount = false;
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
        //window.draw(sprInstructionSpace);
    }

    // draw mouse drag arrow
    if (dragging && canFling) {
        sf::Vector2f dir = mouse - mouseDragStart;
        float mag = util::len(dir) / 15.f;
        float scale = fmin(mag / BALL_MAX_LAUNCH_SPEED, 1.f);
        float ang = atan2f(dir.y, dir.x) * (180.f / PI_F);
        // draw arrow
        sf::Sprite sprArrow(txtArrow);
        sprArrow.setOrigin(sf::Vector2f(0.f, (float)txtArrow.getSize().y / 2.f));
        if (drawArrowOnBall)
            sprArrow.setPosition(ballRestPos - camera);
        else
            sprArrow.setPosition(mouseDragStart);
        sprArrow.setScale(sf::Vector2f(scale, 1.f));
        sprArrow.setRotation(ang);
        sprArrow.setColor(sf::Color(255, 255, 255, 128));
        window.draw(sprArrow);
    }

    // draw nudge indicator
    if (canNudge && !canFling) {
        sf::Vector2f mouse;
        mouse.x = (float)sf::Mouse::getPosition(window).x;
        mouse.y = (float)sf::Mouse::getPosition(window).y;
        sf::Vector2f dir = mouse - (ballRestPos - camera);
        float ang = atan2f(dir.y, dir.x) * (180.f / PI_F);
        sf::Sprite sprNudge(txtBallNudge);
        sprNudge.setOrigin(sf::Vector2f((float)txtBallNudge.getSize().x / 2.f, (float)txtBallNudge.getSize().y / 2.f));
        sprNudge.setPosition(ballRestPos - camera);
        sprNudge.setRotation(ang);
        sprNudge.setColor(sf::Color(99, 155, 255, 128));
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sprNudge.setColor(sf::Color(99, 155, 255));
            sprNudge.setScale(sf::Vector2f(1.2f, 1.2f));
        }
        window.draw(sprNudge);
    }

    // wind
    draw_vector(sf::Vector2f(WINDOW_WIDTH / 2.f, 64.f), currentWind, util::len(currentWind) * 600.f, sf::Color::Blue, window);

    // set cursors
    if (canNudge) set_cursor(CURSOR_CROSS);
    if (canFling) set_cursor(CURSOR_RETICLE);
    if (!canNudge && !canFling) set_cursor(CURSOR_STOP);

    if (drawMoveCount) {
        sf::Text txtCounter(util::to_string(moveCount), fntCounter);
        txtCounter.setPosition(sf::Vector2f(20.f, 10.f));
        txtCounter.setColor(sf::Color::Black);
        window.draw(txtCounter);
    }

    //window.resetGLStates();
    if (edit && ImGui::CollapsingHeader("Hud")) {
        ImGui::Checkbox("draw arrow on ball", &drawArrowOnBall);
        ImGui::Checkbox("draw move count", &drawMoveCount);
    }
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