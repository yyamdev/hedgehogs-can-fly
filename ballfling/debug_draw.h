#pragma once

// routines for drawing useful things

#include <SFML/Graphics.hpp>

extern bool edit;

void draw_vector(const sf::Vector2f &pos, const sf::Vector2f &dir, float len, const sf::Color &col, sf::RenderWindow &window);