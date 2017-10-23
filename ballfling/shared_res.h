#pragma once

// shared resources loaded at launch

#include <SFML/Graphics.hpp>

extern sf::Font fntUi;

extern sf::Texture txtStart;
extern sf::Texture txtBye;

void load_shared_res();
