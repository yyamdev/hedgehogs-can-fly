#include <iostream>
#include <SFML/Graphics.hpp>
#include "hud.h"
#include "debug_draw.h"
#include "util.h"
#include "build_options.h"
#include "ball_entity.h"
#include "cursor.h"
#include "options.h"

const float rad_to_deg = 180.f / 3.14159227f;

Hud::Hud()
{
    Subject::add_observer(this);

    txtArrow.loadFromFile("data/arrow.png");
    sprArrow.setTexture(txtArrow);
    sprArrow.setOrigin(0.f, (float)txtArrow.getSize().y / 2.f);

    txtNudge.loadFromFile("data/ball_nudge.png");
    sprNudge.setTexture(txtNudge);
    sprNudge.setOrigin((float)txtNudge.getSize().x / 2.f, (float)txtNudge.getSize().y / 2.f);
}

void Hud::draw(sf::RenderWindow &window, sf::Vector2f camera, sf::Color levelColour)
{
    if (!ball) return;

    sf::Vector2f mouse;
    mouse.x = (float)sf::Mouse::getPosition(window).x;
    mouse.y = (float)sf::Mouse::getPosition(window).y;

    // Draw arrow when dragging mouse
    if (dragging && ball->can_fling()) {
        // Calculate the speed the ball WILL be flung at
        float speed = 0.f;
        sf::Vector2f dir = mouse - mouseDragStart;
        if (ball->is_on_sand()) {
            speed = util::len(dir) / 25.f;
            speed = util::clamp(speed, 0.f, BALL_MAX_LAUNCH_SPEED_NERF);
        } else {
            speed = util::len(dir) / 15.f;
            speed = util::clamp(speed, 0.f, BALL_MAX_LAUNCH_SPEED);
        }

        power = speed / BALL_MAX_LAUNCH_SPEED;

        sprArrow.setPosition(ball->position - camera);
        sprArrow.setScale(sf::Vector2f(fmin(1.f, power), 1.f));
        sprArrow.setRotation(atan2f(dir.y, dir.x) * rad_to_deg);
        sprArrow.setColor(sf::Color(255, 255, 255, 128));
        window.draw(sprArrow);
    }

    // Draw power bar
    if ((bool)options.power) {
        sf::RectangleShape powerBar;
        float x = 128.f, y = 10.f, width = 256.f, height = 16.f;
        sf::Uint8 alpha = dragging ? 198 : 64;

        // Background
        powerBar.setPosition(x, y);
        powerBar.setSize(sf::Vector2f(width, height));
        powerBar.setFillColor(sf::Color(128,128,128, alpha));
        powerBar.setOutlineThickness(2.f);
        powerBar.setOutlineColor(sf::Color(255, 255, 255, alpha));
        window.draw(powerBar);

        // Previous power
        powerBar.setSize(sf::Vector2f(width * powerPrev, height));
        powerBar.setFillColor(sf::Color(128, 0, 0, alpha));
        window.draw(powerBar);

        // Current power
        powerBar.setSize(sf::Vector2f(width * power, height));
        powerBar.setFillColor(sf::Color(255, 0, 0, alpha));
        window.draw(powerBar);
    }

    // Draw nudge indicator
    if (ball->can_nudge() && !ball->can_fling()) {
        sf::Vector2f dir = mouse - (ball->position - camera);
        sprNudge.setPosition(ball->position - camera);
        sprNudge.setRotation(atan2f(dir.y, dir.x) * rad_to_deg);
        sprNudge.setColor(sf::Color(levelColour.r, levelColour.g, levelColour.b, 128));
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sprNudge.setColor(levelColour);
            sprNudge.setScale(sf::Vector2f(1.2f, 1.2f));
        }
        window.draw(sprNudge);
    }

    // set cursors
    if (ball->can_nudge()) set_cursor(CURSOR_CROSS);
    if (ball->can_fling()) set_cursor(CURSOR_RETICLE);
}

void Hud::on_notify(Event event, void *data) {
    if (event == EVENT_START_DRAG) {
        mouseDragStart = *((sf::Vector2f*)data);
        dragging = true;
    }
    if (event == EVENT_END_DRAG) {
        dragging = false;
        powerPrev = power;
    }
    if (event == EVENT_HUD_SET_ACTIVE_BALL) {
        ball = (EntityBall*)data;
    }
}
