#include <iostream>
#include "SFGUI/Button.hpp"
#include "SFGUI/Label.hpp"
#include "SFGUI/Window.hpp"
#include "SFGUI/Box.hpp"
#include "SFGUI/Separator.hpp"
#include "build_options.h"
#include "util.h"
#include "win_state.h"
#include "world.h"
#include "gui.h"
#include "play_state.h"
#include "save.h"
#include "end_state.h"
#include "timer_entity.h"

StateWin::StateWin(World *world, int levelNum, sf::Color clear, unsigned int millis) :
    State(world, "win"), levelNum(levelNum), clear(clear), millis(millis)
{
    savegame_level_unlock(levelNum + 1);
}

sf::Color StateWin::get_clear_colour()
{
    return clear;
}

void StateWin::on_event(sf::Event &event) {}

void StateWin::on_tick() {}

void StateWin::on_draw(sf::RenderWindow &window) {}

void StateWin::on_draw_ui(sf::RenderWindow &window) {}

void StateWin::on_gain_focus()
{
    gui.RemoveAll();

    world->set_pause(true);

    // Create GUI.
    auto guiWinMain = sfg::Window::Create(sfg::Window::Style::BACKGROUND);
    guiWinMain->SetId("winWinMain");
    gui.Add(guiWinMain);

    auto guiBoxMain = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.f);
    guiWinMain->Add(guiBoxMain);

    auto guiLblTitle = sfg::Label::Create("Level Complete");
    guiLblTitle->SetId("lblWinTitle");
    guiBoxMain->Pack(guiLblTitle);

    auto guiLblTime = sfg::Label::Create("TIME: " + EntityTimer::get_formatted_time_str(millis));
    guiLblTime->SetId("lblWinTime");
    guiBoxMain->Pack(guiLblTime);

    auto guiBoxSpace = sfg::Box::Create();
    guiBoxSpace->SetRequisition(sf::Vector2f(1.f, 50.f));
    guiBoxMain->Pack(guiBoxSpace);

    auto guiBoxButtons = sfg::Box::Create();
    guiBoxMain->Pack(guiBoxButtons);

    if (levelNum != 12) {
        auto guiButtonBack = sfg::Button::Create("Next");
        guiButtonBack->SetId("btnWinNext");
        guiButtonBack->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
                    /*
                     * Save the next level number and world pointer as member
                     * variables cannot be accessed after this instance is
                     * deleted.
                     */
                    int nextLevel = levelNum + 1;
                    World *worldPtr = world;

                    // Go back to level select and push new play state on top.
                    while (State::get_current()->get_name() != "select")
                        State::pop_state();
                    State::push_state(new StatePlay(worldPtr, nextLevel));
                }));
        guiBoxButtons->Pack(guiButtonBack);


        auto guiButtonSelect = sfg::Button::Create("Level Select");
        guiButtonSelect->SetId("btnWinSelect");
        guiButtonSelect->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([] (void) {
                    // Go back to level select state.
                    while (State::get_current()->get_name() != "select")
                        State::pop_state();
                }));
        guiBoxButtons->Pack(guiButtonSelect);

        auto guiButtonResert = sfg::Button::Create("Restart");
        guiButtonResert->SetId("btnWinRestart");
        guiButtonResert->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
                    notify(EVENT_PLAY_SIGNAL_LEVEL_RESTART, NULL);
                    State::pop_state();
                }));
        guiBoxButtons->Pack(guiButtonResert);

    } else {
        auto guiButtonBack = sfg::Button::Create("?");
        guiButtonBack->SetId("btnWinNext");
        guiButtonBack->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
                    // Go back to level select and push end game state on top.
                    while (State::get_current()->get_name() != "select")
                        State::pop_state();
                    State::change_state(new StateEnd(world));
                }));
        guiBoxButtons->Pack(guiButtonBack);
    }

    /*
     * Position window at centre of screen. This needs to be done at the end so
     * SFGUI knows how big it has to be.
     */
    guiWinMain->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2 - guiWinMain->GetRequisition().x / 2.f, WINDOW_HEIGHT / 2 - guiWinMain->GetRequisition().y / 2.f - 75.f));
}

void StateWin::on_lose_focus()
{
    world->set_pause(false);
}
