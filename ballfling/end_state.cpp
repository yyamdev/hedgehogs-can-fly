#include <iostream>
#include "SFGUI/Button.hpp"
#include "end_state.h"
#include "world.h"
#include "gui.h"
#include "build_options.h"
#include "util.h"
#include "play_state.h"
#include "fireworks_entity.h"
#include "particle.h"
#include "shared_res.h"
#include "cursor.h"

StateEnd::StateEnd(World *world) : State(world, "end")
{
}

void StateEnd::on_event(sf::Event &event)
{
}

sf::Color StateEnd::get_clear_colour()
{
    return sf::Color::Black;
}

void StateEnd::on_gain_focus()
{
    gui.RemoveAll();
    guiButtonSelect = sfg::Button::Create("Back");
    guiButtonSelect->SetId("btnDeathSelect");
    guiButtonSelect->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([] (void) {
        State::pop_state(); // Pop state back to level select
    }));
    guiButtonSelect->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2 - guiButtonSelect->GetRequisition().x / 2.f, 500.f));

    timerFirework.restart();
    timerState.restart();
    set_cursor_visible(true);
    set_cursor(CURSOR_POINTER);
    notify(EVENT_ENTER_END_SCREEN, NULL);
}

void StateEnd::on_tick()
{
    particles_tick(sf::Vector2f(0.f, 0.f));

    if (state == 0) {
        if (timerFirework.getElapsedTime().asSeconds() > fireworkTime) {
            EntityFireworks::do_fireworks(sf::Vector2f(util::rnd(0.f, (float)WINDOW_WIDTH),
                                                       util::rnd(0.f, (float)WINDOW_HEIGHT)),
                                          50);
            timerFirework.restart();
            fireworkTime -= .25f;
            if (fireworkTime < .2f)
                fireworkTime = .2f;
        }

        if (timerState.getElapsedTime().asSeconds() > 12.f) {
            state = 1;
            nextFireworkSeconds = util::rnd(.8f, 2.5f);
            gui.Add(guiButtonSelect);
        }
    }

    if (state == 1) {
        if (timerFirework.getElapsedTime().asSeconds() > nextFireworkSeconds) {
            EntityFireworks::do_fireworks(sf::Vector2f(util::rnd(0.f, (float)WINDOW_WIDTH),
                                                       util::rnd(0.f, (float)WINDOW_HEIGHT)),
                                          50);
            timerFirework.restart();
            nextFireworkSeconds = util::rnd(.8f, 2.5f);
        }
    }
}

void StateEnd::on_draw(sf::RenderWindow &window)
{
    particles_draw(window, sf::Vector2f(0.f, 0.f));
    if (state == 1) {
        sf::Sprite sprMsg(txtBye);
        window.draw(sprMsg);
    }
}
