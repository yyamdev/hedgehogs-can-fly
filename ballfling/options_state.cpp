#include <iostream>
#include "SFGUI/Button.hpp"
#include "SFGUI/Scale.hpp"
#include "SFGUI/Label.hpp"
#include "SFGUI/Window.hpp"
#include "SFGUI/Box.hpp"
#include "SFGUI/Separator.hpp"
#include "SFGUI/CheckButton.hpp"
#include "options_state.h"
#include "world.h"
#include "gui.h"
#include "build_options.h"
#include "util.h"
#include "options.h"
#include "save.h"
#include "window.h"

StateOptions::StateOptions(World *world, bool showEraseSave, sf::Color clear) :
    State(world, "options"), showEraseSave(showEraseSave), clear(clear)
{
    fullscreenPrev = (bool)options.fullscreen;
}

sf::Color StateOptions::get_clear_colour()
{
    return clear;
}

void StateOptions::on_event(sf::Event &event)
{
}

void StateOptions::on_tick()
{
    if (fullscreenPrev != guiToggleFullscreen->IsActive()) {
        fullscreenPrev = !fullscreenPrev;
        reconfigure_window(fullscreenPrev);
    }
}

void StateOptions::on_draw(sf::RenderWindow &window)
{
}

void StateOptions::on_gain_focus()
{
    const sf::Vector2f sliderSize(200.f, 32.f);

    gui.RemoveAll();

    options.load(CONFIG_FILENAME);

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
                // Ensure volume change can be heard with slider movement
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

    guiToggleTrail = sfg::CheckButton::Create("");
    guiToggleTrail->SetId("tglOptionsTrail");
    guiToggleTrail->SetActive((bool)options.trail);
    guiBoxTrail->Pack(guiToggleTrail);

    auto guiBoxPower = sfg::Box::Create();
    guiBoxMain->Pack(guiBoxPower);

    auto guiLblPower = sfg::Label::Create("Power Indicator");
    guiLblPower->SetId("lblOptionsPower");
    guiBoxPower->Pack(guiLblPower);

    guiTogglePower = sfg::CheckButton::Create("");
    guiTogglePower->SetId("tglOptionsPower");
    guiTogglePower->SetActive((bool)options.power);
    guiBoxPower->Pack(guiTogglePower);

    auto guiBoxFullscreen = sfg::Box::Create();
    guiBoxMain->Pack(guiBoxFullscreen);

    auto guiLblFullscreen = sfg::Label::Create("Fullscreen");
    guiLblFullscreen->SetId("lblOptionsFullscreen");
    guiBoxFullscreen->Pack(guiLblFullscreen);

    guiToggleFullscreen = sfg::CheckButton::Create("");
    guiToggleFullscreen->SetId("tglOptionsFullscreen");
    guiToggleFullscreen->SetActive((bool)options.fullscreen);
    guiBoxFullscreen->Pack(guiToggleFullscreen);

    auto guiBoxEdges = sfg::Box::Create();
    guiBoxMain->Pack(guiBoxEdges);

    auto guiLblEdges = sfg::Label::Create("Render Terrain Edges");
    guiLblEdges->SetId("lblOptionsEdges");
    guiBoxEdges->Pack(guiLblEdges);

    guiToggleEdges = sfg::CheckButton::Create("");
    guiToggleEdges->SetId("tglOptionsEdges");
    guiToggleEdges->SetActive((bool)options.edges);
    guiBoxEdges->Pack(guiToggleEdges);

    if (showEraseSave) {
        auto guiBoxErase = sfg::Box::Create();
        guiBoxMain->Pack(guiBoxErase);

        auto guiButtonBack = sfg::Button::Create("Erase Save(!)");
        guiButtonBack->SetId("btnOptionsErase");
        guiButtonBack->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
                    notify(EVENT_MENU_CLICK, NULL);
                    savegame_reset();
        }));
        guiBoxErase->Pack(guiButtonBack);
    }

    auto guiButtonBack = sfg::Button::Create("Back");
    guiButtonBack->SetId("btnOptionsBack");
    guiButtonBack->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2.f - guiButtonBack->GetRequisition().x / 2.f, 500.f));
    guiButtonBack->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([this] (void) {
                notify(EVENT_MENU_CLICK, NULL);
                State::pop_state();
    }));
    guiBoxMain->Pack(guiButtonBack);

    // Position window at centre of screen
    guiWinMain->SetPosition(sf::Vector2f(WINDOW_WIDTH / 2 - guiWinMain->GetRequisition().x / 2.f, WINDOW_HEIGHT / 2 - guiWinMain->GetRequisition().y / 2.f));
}

void StateOptions::on_lose_focus()
{
    options.musicVolume = (double)guiSliderMusic->GetValue();
    options.sfxVolume = (double)guiSliderSfx->GetValue();
    options.trail = (int)guiToggleTrail->IsActive();
    options.power = (int)guiTogglePower->IsActive();
    options.fullscreen = (int)guiToggleFullscreen->IsActive();
    options.edges = (int)guiToggleEdges->IsActive();
    options.save(CONFIG_FILENAME);
}
