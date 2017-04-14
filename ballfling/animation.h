#pragma once

// Keeps track of which frame an animation is on

#include <SFML/System.hpp>

class Animation {
public:
    Animation();
    Animation(int frames, int fps);
    int get_frame();
private:
    sf::Clock clock;
    int frames;
    int fps;
};