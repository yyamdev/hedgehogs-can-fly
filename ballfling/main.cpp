#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>
#include "SFGUI/SFGUI.hpp"
#include "SFGUI/Button.hpp"
#include "SFGUI/Window.hpp"
#include "SFGUI/Desktop.hpp"
#include "imgui.h"
#include "imgui-SFML.h"
#include "build_options.h"
#include "util.h"
#include "debug_draw.h"
#include "shared_res.h"
#include "world.h"
#include "gui.h"
#include "cursor.h"
#include "particle.h"
#include "audio.h"
#include "splash_state.h"

#include <windows.h>

sfg::SFGUI guiManager;
sfg::Desktop gui;

// Only open console window in debug mode
#ifndef _DEBUG
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
#else
int main()
#endif
{
    if (!sf::Shader::isAvailable())
    {
        MessageBox(NULL, "Oops.. It looks like your graphics card does not support shaders :( The game needs shader support in order to run. Make sure you have the most up to date graphics drivers installed!", "Error", 0);
        return -1;
    }

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Hedgehogs Can Fly", sf::Style::Close);
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);

    std::cout << "OpenGL version: " << window.getSettings().majorVersion << "." << window.getSettings().minorVersion << std::endl;
    std::cout << "debug controls:\n";
    std::cout << "F1 - screenshot\n";
    std::cout << "F2 - toggle tools mode\n";

    load_shared_res();
    load_cursor_textures();
    gui.LoadThemeFromFile("data/ui.css");

    Audio audio;
    World world(window);
    ImGui::SFML::Init(window);

    State::change_state(new StateSplash(&world));

    sf::Clock imguiDelta;
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            ImGui::SFML::ProcessEvent(e);
            gui.HandleEvent(e);

            if (e.type == sf::Event::Closed) {
                window.close();
            }

            #ifdef _DEBUG
            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::F2) {
                edit = !edit;
                window.setMouseCursorVisible(edit);
            }
            #endif

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::F1) {
                window.capture().saveToFile(util::to_string(time(NULL)) + ".png");
            }

            world.event(e);
            State::event_current(e);
        }

        // Logic
        gui.Update(1.f / 60.f);
        audio.tick_game_music();
        world.tick();
        State::tick_current();
        ImGui::SFML::Update(window, imguiDelta.restart());

        // Rendering
        window.clear(State::get_current()->get_clear_colour());
        world.draw();
        State::draw_current(window);
        State::draw_ui_current(window);
        guiManager.Display(window);

        /*
         * The next line keeps the terrain rendering properly when the ImGui
         * header is active. It seems like there are some OpenGL state
         * incompatibilities with SFML and ImGui?
         */
        window.resetGLStates();

        if (edit && ImGui::CollapsingHeader("Resources")) {
            if (ImGui::Button("reload sfx")) {
                audio.reload_sfx();
            }
        }

        ImGui::Render();
        draw_cursor(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
