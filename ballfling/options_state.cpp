#include "options_state.h"
#include "world.h"
#include <iostream>
#include "gui.h"
#include "SFGUI/Button.hpp"
#include "SFGUI/Scale.hpp"
#include "build_options.h"
#include "util.h"
#include "options.h"
#include "SFGUI/Label.hpp"
#include "SFGUI/Frame.hpp"
#include "SFGUI/Window.hpp"
#include "SFGUI/Box.hpp"

StateOptions::StateOptions(World *world) : State(world) {
}

void StateOptions::on_event(sf::Event &event) {
}

void StateOptions::on_tick() {
}

void StateOptions::on_draw(sf::RenderWindow &window) {
}

void StateOptions::on_draw_ui(sf::RenderWindow &window) {
}

void StateOptions::on_gain_focus() {
    // remove all entities in world & clear ui
    world->remove_entity(ENTITY_TAG_ALL);
    gui.RemoveAll();

    options.load();

    // create ui
    sf::Vector2f sliderSize(200.f, 32.f);

    auto guiWinMain = sfg::Window::Create(sfg::Window::Style::NO_STYLE);
    gui.Add(guiWinMain);

    auto guiBoxMain = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.f);
    guiWinMain->Add(guiBoxMain);

    auto guiBoxMusic = sfg::Box::Create();
    guiBoxMain->Pack(guiBoxMusic);

    auto guiLblMusic = sfg::Label::Create("Music");
    guiLblMusic->SetId("lblOptionsMusic");
    guiBoxMusic->Pack(guiLblMusic);

    auto guiSliderMusic = sfg::Scale::Create(0.f, 1.f, 0.01f);
    guiSliderMusic->SetId("sclOptionsMusic");
    guiSliderMusic->SetRequisition(sliderSize);
    guiSliderMusic->SetValue(options.musicVolume);
    guiSliderMusic->GetSignal(sfg::Scale::OnMouseLeftRelease).Connect(std::bind([guiSliderMusic] (void) {
        options.musicVolume = guiSliderMusic->GetValue();
    }));
    guiBoxMusic->Pack(guiSliderMusic);

    auto guiBoxSfx = sfg::Box::Create();
    guiBoxMain->Pack(guiBoxSfx);

    auto guiLblSfx = sfg::Label::Create("SFX");
    guiLblSfx->SetId("lblOptionsSfx");
    guiBoxSfx->Pack(guiLblSfx);

    auto guiSliderSfx = sfg::Scale::Create(0.f, 1.f, 0.01f);
    guiSliderSfx->SetId("sclOptionsSfx");
    guiSliderSfx->SetRequisition(sliderSize);
    guiSliderSfx->SetValue(options.sfxVolume);
    guiSliderSfx->GetSignal(sfg::Scale::OnMouseLeftRelease).Connect(std::bind([guiSliderSfx] (void) {
        options.musicVolume = guiSliderSfx->GetValue();
    }));
    guiBoxSfx->Pack(guiSliderSfx);

    auto guiButtonBack = sfg::Button::Create("Back");
    guiButtonBack->SetId("btnOptionsBack");
    guiButtonBack->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiButtonBack->GetRequisition().x / 2.f, 500.f));
    gui.Add(guiButtonBack);
    guiButtonBack->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([guiSliderMusic, guiSliderSfx] (void) {
        guiSliderMusic->Show(false); // neccessary to stop this widget from displaying in other menus, possibly an SFGUI bug?
        guiSliderSfx->Show(false);   // ""
        State::pop_state();
    }));

    // position window at centre of screen
    // needs to be done at the end so SFGUI knows how big it has to be
    guiWinMain->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2 - guiWinMain->GetRequisition().x / 2.f, WINDOW_HEIGHT / 2 - guiWinMain->GetRequisition().y / 2.f - 75.f));
}

void StateOptions::on_lose_focus() {
}

void StateOptions::on_notify(Event event, void *data) {
}
