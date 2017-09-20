#pragma once

// small module that draws different cursors

#include "SFML/Graphics.hpp"

namespace _cursors {
    extern sf::Texture txtCursorReticle;
    extern sf::Texture txtCursorCross;
    extern sf::Texture txtCursorPointer;
    extern sf::Texture txtCursorStop;
    extern sf::Sprite sprCursor;
    extern bool draw;
}

// macros for easy reference to cursor textures
#define CURSOR_RETICLE (&_cursors::txtCursorReticle)
#define CURSOR_CROSS (&_cursors::txtCursorCross)
#define CURSOR_POINTER (&_cursors::txtCursorPointer)
#define CURSOR_STOP (&_cursors::txtCursorStop)

void load_cursor_textures();
void draw_cursor(sf::RenderWindow &window);
void set_cursor(sf::Texture *cursorTexture);
void set_cursor_visible(bool visible);
