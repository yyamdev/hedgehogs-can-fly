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
#include <fstream>
#include "particle.h"

// frame time profiler
#define PBUFLEN 16
sf::Int32 pbuf[PBUFLEN];
int pbufi = 0;
float framePercent = 0.f;
float framePercentBuf[PBUFLEN];
int fbufi = 0;

sfg::Desktop gui; // SFGUI global Desktop object

void print_debug_controls();

#define CSS_BUF_SIZE 8192
char cssBuf[CSS_BUF_SIZE];

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), TITLE, sf::Style::Close);
    window.setFramerateLimit(60);

    load_shared_res();
    load_cursor_textures();

    std::cout << "OpenGL version " << window.getSettings().majorVersion << "." << window.getSettings().minorVersion << std::endl;
    print_debug_controls();

    World world(window);

    sfg::SFGUI guiManager;
    gui.LoadThemeFromFile("data/ui.css");

    State::change_state(new StateMenu(&world));

    ImGui::SFML::Init(window);
    window.setMouseCursorVisible(false);
    sf::Clock imguiDelta;
    sf::Clock clkProfile;
    window.resetGLStates();
    while (window.isOpen()) {
        clkProfile.restart();
        sf::Event e;
        while (window.pollEvent(e)) {
            ImGui::SFML::ProcessEvent(e);
            gui.HandleEvent(e);

            if (e.type == sf::Event::Closed) {
                window.close();
            }

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::F2) {
                edit = !edit;
                window.setMouseCursorVisible(edit);
                std::ifstream fileTheme("data/ui.css");
                fileTheme.get(cssBuf, CSS_BUF_SIZE, 0);
                fileTheme.close();
            }

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::F1)
                window.capture().saveToFile(util::to_string(time(NULL)) + ".png");

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::F3)
                State::pop_state();

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::F4)
                system("cls");
            
            world.event(e);
            State::event_current(e);
        }

        gui.Update(1.f / 60.f);
        world.tick();
        State::tick_current();
        float pSize = util::rnd(1.f, 15.f);
        particleSystem.add_particle(Particle(sf::Vector2f(300.f, 400.f), util::choose(sf::Color::Blue, sf::Color::Red), sf::Vector2f(util::rnd(-2.f, 2.f), util::rnd(-6.f, -4.f)), sf::Vector2f(pSize, pSize)));
        particleSystem.tick();
        
        window.clear(sf::Color(153, 217, 234));
        ImGui::SFML::Update(window, imguiDelta.restart());
        world.draw();
        State::draw_current(window);
        State::draw_ui_current(window);
        guiManager.Display(window);
        particleSystem.draw(window);
        
        window.resetGLStates();
        
        if (edit && ImGui::CollapsingHeader("Performance")) {
            sf::Color frameCol = sf::Color::Green;
            if (framePercent > 100.f) frameCol = sf::Color::Red;
            ImGui::TextColored(frameCol, "frame time: %f%", framePercent);
            ImGui::PlotLines("frame time", framePercentBuf, PBUFLEN, 0, 0, 95.f, 140.f, sf::Vector2f(0, 70.f), 4);
        }
        if (edit && ImGui::CollapsingHeader("CSS")) {
            if (ImGui::Button("Update")) {
                std::ofstream fileTheme("data/ui.css");
                for (int i = 0; i < CSS_BUF_SIZE; ++i) {
                    if (cssBuf[i] == 0) break;
                    fileTheme.put(cssBuf[i]);
                }
                fileTheme.close();
                gui.LoadThemeFromFile("data/ui.css");
            }
            if (ImGui::Button("Reload")) {
                gui.LoadThemeFromFile("data/ui.css");
            }
            ImGui::InputTextMultiline("css", cssBuf, CSS_BUF_SIZE, sf::Vector2f(400.f, 250.f), ImGuiInputTextFlags_AllowTabInput);
        }
        ImGui::Render();
        draw_cursor(window);
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
    std::cout << "F1 - screenshot\n";
    std::cout << "F2 - toggle tools mode\n";
    std::cout << "F3 - pop current state\n";
    std::cout << "F4 - clear console\n";
    std::cout << "F5 - toggle pause\n";
}