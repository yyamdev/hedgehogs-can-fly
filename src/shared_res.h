#pragma once

/*
 * Shared resources loaded at game startup
 */

#include <SFML/Graphics.hpp>

extern sf::Font fntUi;

extern sf::Texture txtStart;
extern sf::Texture txtBye;

void load_shared_res();
