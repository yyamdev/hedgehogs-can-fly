#pragma once

// plays audio
#include "observer.h"
#include <SFML/Audio.hpp>

class Audio : public Observer {
public:
    Audio();
    void stop_all_music();
    void update_all_music_volume();
    void on_notify(Event event, void *data);
private:
    sf::SoundBuffer bufSplash;
    sf::Sound sndSplash;

    sf::SoundBuffer bufBounce;
    sf::Sound sndBounce;

    sf::SoundBuffer bufSmash;
    sf::Sound sndSmash;

    sf::SoundBuffer bufHit;
    sf::Sound sndHit;
    sf::Clock clkHit;

    int currentlyPlaying;
    /*
     *  0  nothing
     *  1  menu
     *  2  game track A
     */
    sf::Music musMenu;
    sf::Music musGameA;
};
