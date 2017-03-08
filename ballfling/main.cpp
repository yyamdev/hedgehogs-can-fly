#include <iostream>
#include <SFML/Graphics.hpp>
#include <memory>
#include "build_options.h"
#include "world.h"
#include "play_state.h"
#include "util.h"
#include "menu_state.h"
#include "shared_res.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include "debug_draw.h"
#include "SFGUI/SFGUI.hpp"
#include "SFGUI/Button.hpp"
#include "SFGUI/Window.hpp"
#include "SFGUI/Desktop.hpp"
#include "gui.h"
#include "cursor.h"

#define GO_TO_TEST_LEVEL 0

// frame time profiler
#define PBUFLEN 16
sf::Int32 pbuf[PBUFLEN];
int pbufi = 0;
float framePercent = 0.f;
float framePercentBuf[PBUFLEN];
int fbufi = 0;

sfg::Desktop gui; // SFGUI global Desktop object

void print_debug_controls();

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), TITLE, sf::Style::Close);
    window.setFramerateLimit(60);

    load_shared_res();
    load_cursor_textures();

    std::cout << "OpenGL version " << window.getSettings().majorVersion << "." << window.getSettings().minorVersion << std::endl;
    print_debug_controls();

    World world(window);

    sfg::SFGUI guiManager;
    auto guiButton = sfg::Button::Create("Hello");
    guiButton->GetSignal(sfg::Button::OnLeftClick).Connect(std::bind([] (void) {
        std::cout << "Click!\n";
    }));
    gui.LoadThemeFromFile("data/example.theme");
    gui.Add(guiButton);
    

    if (GO_TO_TEST_LEVEL)
        State::change_state(new StatePlay(&world, "data/map.png"));
    else
        State::change_state(new StateMenu(&world));

    ImGui::SFML::Init(window);
    window.setMouseCursorVisible(false);
    sf::Clock imguiDelta;
    sf::Clock clkProfile;
    while (window.isOpen()) {
        clkProfile.restart();
        sf::Event e;
        while (window.pollEvent(e)) {
            ImGui::SFML::ProcessEvent(e);
            gui.HandleEvent(e);

            if (e.type == sf::Event::Closed) {
                window.close();
            }

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::E) {
                edit = !edit;
            }

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::F1)
                window.capture().saveToFile(util::to_string(time(NULL)) + ".png");

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::R)
                State::change_state(new StatePlay(&world, "data/map.png"));

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::H)
                print_debug_controls();

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::C)
                system("cls");
            
            world.event(e);
            State::event_current(e);
        }

        ImGui::SFML::Update(window, imguiDelta.restart());
        gui.Update(1.f / 60.f);
        window.setMouseCursorVisible(edit);
        world.tick();
        State::tick_current();
        
        window.clear(sf::Color(153, 217, 234));
        world.draw();
        State::draw_current(window);
        State::draw_ui_current(window);
        guiManager.Display(window);
        draw_cursor(window);
        if (edit && ImGui::CollapsingHeader("Performance")) {
            sf::Color frameCol = sf::Color::Green;
            if (framePercent > 100.f) frameCol = sf::Color::Red;
            ImGui::TextColored(frameCol, "frame time: %f%", framePercent);
            ImGui::PlotLines("frame time", framePercentBuf, PBUFLEN, 0, 0, 95.f, 140.f, sf::Vector2f(0, 70.f), 4);
        }
        ImGui::Render();
        window.display();

        pbuf[pbufi] = clkProfile.getElapsedTime().asMilliseconds();
        pbufi = (pbufi + 1) % PBUFLEN;
        if (pbufi == 0) {
            sf::Int32 av = 0;
            for (int i = 0; i < PBUFLEN; ++i) {
                av += pbuf[i];
            }
            framePercent = (float)(av / PBUFLEN) / 16 * 100.f;
            framePercentBuf[fbufi] = framePercent;
            fbufi = (fbufi + 1) % PBUFLEN;
        }
    }

    ImGui::SFML::Shutdown();
    State::free_memory();
    return 0;
}

void print_debug_controls() {
    std::cout << "debug controls:\n";
    std::cout << "B - toggle pause\n";
    std::cout << "F1 - screenshot\n";
    std::cout << "R - restart game\n";
    std::cout << "H - print this message\n";
    std::cout << "E - toggle tools mode\n";
    std::cout << "C - clear console\n";
}