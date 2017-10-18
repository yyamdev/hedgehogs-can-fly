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
#include "SFGUI/Window.hpp"
#include "SFGUI/Box.hpp"
#include "SFGUI/Separator.hpp"
#include "SFGUI/CheckButton.hpp"
#include "save.h"

StateOptions::StateOptions(World *world, bool full) : State(world) {
    this->full = full;
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
    //world->remove_entity(ENTITY_TAG_ALL);
    gui.RemoveAll();

    options.load(CONFIG_FILENAME);

    // create ui
    sf::Vector2f sliderSize(200.f, 32.f);

    auto guiWinMain = sfg::Window::Create(sfg::Window::Style::BACKGROUND);
    guiWinMain->SetId("winOptionsMain");
    gui.Add(guiWinMain);

    auto guiBoxMain = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.f);
    guiWinMain->Add(guiBoxMain);

    auto guiLblTitle = sfg::Label::Create("Options");
    guiLblTitle->SetId("lblOptionsTitle");
    guiBoxMain->Pack(guiLblTitle);

    auto guiBoxSpace = sfg::Box::Create();
    guiBoxSpace->SetRequisition(sf::Vector2f(1.f, 50.f));
    guiBoxMain->Pack(guiBoxSpace);

    auto guiBoxMusic = sfg::Box::Create();
    guiBoxMain->Pack(guiBoxMusic);

    auto guiLblMusic = sfg::Label::Create("Music");
    guiLblMusic->SetId("lblOptionsMusic");
    guiBoxMusic->Pack(guiLblMusic);

    guiSliderMusic = sfg::Scale::Create(0.f, 1.f, 1/64.f);
    guiSliderMusic->SetId("sclOptionsMusic");
    guiSliderMusic->SetRequisition(sliderSize);
    guiSliderMusic->SetValue((float)options.musicVolume);
    guiSliderMusic->GetSignal(sfg::Scale::OnMouseMove).Connect(std::bind([this] (void) {
                // Hack so users can hear music volume change with slider movement
                options.musicVolume = (double)guiSliderMusic->GetValue();
                notify(EVENT_CHANGE_MUSIC_VOLUME, NULL);
        }));
    guiBoxMusic->Pack(guiSliderMusic);

    auto guiBoxSfx = sfg::Box::Create();
    guiBoxMain->Pack(guiBoxSfx);

    auto guiLblSfx = sfg::Label::Create("SFX");
    guiLblSfx->SetId("lblOptionsSfx");
    guiBoxSfx->Pack(guiLblSfx);

    guiSliderSfx = sfg::Scale::Create(0.f, 1.f, 1/64.f);
    guiSliderSfx->SetId("sclOptionsSfx");
    guiSliderSfx->SetRequisition(sliderSize);
    guiSliderSfx->SetValue((float)options.sfxVolume);
    guiBoxSfx->Pack(guiSliderSfx);

    auto guiBoxTrail = sfg::Box::Create();
    guiBoxMain->Pack(guiBoxTrail);

    auto guiLblTrail = sfg::Label::Create("Trail");
    guiLblTrail->SetId("lblOptionsTrail");
    guiBoxTrail->Pack(guiLblTrail);

    guiToggleTrail = sfg::CheckButton::Create("Trail");
    guiToggleTrail->SetId("tglOptionsTrail");
    guiToggleTrail->SetActive((bool)options.trail);
    guiBoxTrail->Pack(guiToggleTrail);

    if (full) {
        auto guiBoxErase = sfg::Box::Create();
        guiBoxMain->Pack(guiBoxErase);
        
        auto guiButtonBack = sfg::Button::Create("Erase Save(!)");
        guiButtonBack->SetId("btnOptionsErase");
        guiButtonBack->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
            savegame_reset();
        }));
        guiBoxErase->Pack(guiButtonBack);
    }

    auto guiButtonBack = sfg::Button::Create("Back");
    guiButtonBack->SetId("btnOptionsBack");
    guiButtonBack->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiButtonBack->GetRequisition().x / 2.f, 500.f));
    guiButtonBack->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
        guiSliderMusic->Show(false); // neccessary to stop this widget from displaying in other menus, possibly an SFGUI bug?
        guiSliderSfx->Show(false);   // ""
        State::pop_state();
    }));
    guiBoxMain->Pack(guiButtonBack);

    // position window at centre of screen
    // needs to be done at the end so SFGUI knows how big it has to be
    guiWinMain->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2 - guiWinMain->GetRequisition().x / 2.f, WINDOW_HEIGHT / 2 - guiWinMain->GetRequisition().y / 2.f));
}

void StateOptions::on_lose_focus() {
    options.musicVolume = (double)guiSliderMusic->GetValue();
    options.sfxVolume = (double)guiSliderSfx->GetValue();
    options.trail = (int)guiToggleTrail->IsActive();
    options.save(CONFIG_FILENAME);
}

void StateOptions::on_notify(Event event, void *data) {
}
