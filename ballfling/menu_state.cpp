#include <iostream>
#include <math.h>
#include "SFGUI/Button.hpp"
#include "SFGUI/Scale.hpp"
#include "SFGUI/Label.hpp"
#include "menu_state.h"
#include "select_state.h"
#include "options_state.h"
#include "world.h"
#include "particle.h"
#include "gui.h"
#include "shared_res.h"
#include "build_options.h"
#include "cursor.h"
#include "util.h"

StateMenu::StateMenu(World *world) : State(world, "menu")
{
    set_cursor_visible(true);
    set_cursor(CURSOR_POINTER);

    txtTitle.loadFromFile("data/header.png");
    sprTitle.setTexture(txtTitle);
    sprTitle.setOrigin(txtTitle.getSize().x / 2.f, txtTitle.getSize().y / 2.f);
    sprTitle.setPosition(WINDOW_WIDTH / 2.f, 150.f);

    txtHog.loadFromFile("data/ball.png");
    sprHog.setTexture(txtHog);
    sprHog.setOrigin(txtHog.getSize().x / 2.f, txtHog.getSize().y / 2.f);
}

void StateMenu::on_event(sf::Event &event)
{
}

void StateMenu::on_tick()
{
    // Animate header text
    static float time = 0.f;
    static float size = 0.6f;
    static float rot  = 0.f;

    size += sin(time) * .002f;
    rot   = sin(time) * 1.6f;
    time += .05f;

    sprTitle.setScale(sf::Vector2f(size, size));
    sprTitle.setRotation(rot);

    // Animate hedgehog
    float speed = 2.f;
    static sf::Vector2f pos(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);
    static sf::Vector2f vel(util::rnd(-speed, speed), util::rnd(-speed, speed));

    sf::Vector2f next = pos + vel;
    if (next.x < 0.f + 10.f || next.x > WINDOW_WIDTH - 10.f)  vel.x = -vel.x;
    if (next.y < 0.f + 10.f || next.y > WINDOW_HEIGHT - 10.f) vel.y = -vel.y;
    pos += vel;
    sprHog.setPosition(pos);

    particles_tick(sf::Vector2f());
}

void StateMenu::on_draw(sf::RenderWindow &window)
{
    particles_draw(window, sf::Vector2f());
    static float angle = 0.f;
    angle += .5f;
    sprHog.setRotation(angle);
    window.draw(sprHog);
    window.draw(sprTitle);

    sf::Text version("version " VERSION_STRING, fntUi, 16);
    version.setPosition(16.f, WINDOW_HEIGHT - version.getCharacterSize() - 16.f);
    version.setColor(sf::Color(128, 128, 128));
    window.draw(version);
}

void StateMenu::on_gain_focus()
{
    world->remove_entity(ENTITY_TAG_ALL);
    gui.RemoveAll();
    notify(EVENT_ENTER_MENU, NULL); // Signal to play music

    // create ui
    auto guiButtonPlay = sfg::Button::Create("Play");
    guiButtonPlay->SetId("btnMenuPlay");
    guiButtonPlay->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiButtonPlay->GetRequisition().x / 2.f, 325.f));
    gui.Add(guiButtonPlay);
    guiButtonPlay->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
                notify(EVENT_MENU_CLICK, NULL);
                State::push_state(new StateSelect(world));
            }));

    auto guiButtonOptions = sfg::Button::Create("Options");
    guiButtonOptions->SetId("btnMenuOptions");
    guiButtonOptions->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiButtonOptions->GetRequisition().x / 2.f, 425.f));
    gui.Add(guiButtonOptions);
    guiButtonOptions->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
                notify(EVENT_MENU_CLICK, NULL);
                State::push_state(new StateOptions(world, true, sf::Color::Black));
            }));
}
