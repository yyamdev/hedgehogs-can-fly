#include "win_state.h"
#include "world.h"
#include <iostream>
#include "gui.h"
#include "SFGUI/Button.hpp"
#include "build_options.h"
#include "util.h"
#include "SFGUI/Label.hpp"
#include "SFGUI/Window.hpp"
#include "SFGUI/Box.hpp"
#include "SFGUI/Separator.hpp"
#include "play_state.h"
#include "save.h"
#include "end_state.h"

StateWin::StateWin(World *world, int levelNum, bool *restartFlag, sf::Color clear) : State(world) {
    this->levelNum = levelNum;
    this->restartFlag = restartFlag;
    this->clear = clear;
    savegame_level_unlock(levelNum + 1);
}

sf::Color StateWin::get_clear_colour()
{
    return clear;
}

void StateWin::on_event(sf::Event &event) {
}

void StateWin::on_tick() {
}

void StateWin::on_draw(sf::RenderWindow &window) {
}

void StateWin::on_draw_ui(sf::RenderWindow &window) {
}

void StateWin::on_gain_focus() {
    gui.RemoveAll();

    if (!world->is_paused()) world->toggle_pause();

    // TODO -> Update save files?

    // create ui
    auto guiWinMain = sfg::Window::Create(sfg::Window::Style::BACKGROUND);
    guiWinMain->SetId("winWinMain");
    gui.Add(guiWinMain);

    auto guiBoxMain = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.f);
    guiWinMain->Add(guiBoxMain);

    auto guiLblTitle = sfg::Label::Create("Level Complete");
    guiLblTitle->SetId("lblWinTitle");
    guiBoxMain->Pack(guiLblTitle);

    auto guiBoxSpace = sfg::Box::Create();
    guiBoxSpace->SetRequisition(sf::Vector2f(1.f, 50.f));
    guiBoxMain->Pack(guiBoxSpace);

    auto guiBoxButtons = sfg::Box::Create();
    guiBoxMain->Pack(guiBoxButtons);

    if (levelNum != 12) {
        auto guiButtonBack = sfg::Button::Create("Next");
        guiButtonBack->SetId("btnWinNext");
        guiButtonBack->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
                    State::push_state(new StatePlay(world, levelNum + 1));
                }));
        guiBoxButtons->Pack(guiButtonBack);

        
        auto guiButtonSelect = sfg::Button::Create("Level Select");
        guiButtonSelect->SetId("btnWinSelect");
        guiButtonSelect->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([] (void) {
                    // pop state back to level select
                    while (State::get_current()->get_name() != "select")
                        State::pop_state();
                }));
        guiBoxButtons->Pack(guiButtonSelect);

        auto guiButtonResert = sfg::Button::Create("Restart");
        guiButtonResert->SetId("btnWinRestart");
        guiButtonResert->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
                    *restartFlag = true;
                    State::pop_state();
                }));
        guiBoxButtons->Pack(guiButtonResert);

    } else {
        auto guiButtonBack = sfg::Button::Create("?");
        guiButtonBack->SetId("btnWinNext");
        guiButtonBack->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
                    // pop state back to level select
                    while (State::get_current()->get_name() != "select")
                        State::pop_state();
                    State::change_state(new StateEnd(world));
                }));
        guiBoxButtons->Pack(guiButtonBack);
    }

    // position window at centre of screen
    // needs to be done at the end so SFGUI knows how big it has to be
    guiWinMain->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2 - guiWinMain->GetRequisition().x / 2.f, WINDOW_HEIGHT / 2 - guiWinMain->GetRequisition().y / 2.f - 75.f));
}

void StateWin::on_lose_focus() {
    if (world->is_paused()) world->toggle_pause();
}

void StateWin::on_notify(Event event, void *data) {
}
