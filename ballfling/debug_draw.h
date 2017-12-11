#pragma once

/*
 * Debugging functions.
 */

#include <SFML/Graphics.hpp>

extern bool edit; // Debug mode on/off

void draw_vector(const sf::Vector2f &pos, const sf::Vector2f &dir, float len, const sf::Color &col, sf::RenderWindow &window);
