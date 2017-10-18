#include "audio.h"
#include <iostream>
#include "options.h"
#include "subject.h"

Audio::Audio() {
    Subject::add_observer(this);
    
    bufSplash.loadFromFile("data/splash.wav");
    sndSplash.setBuffer(bufSplash);

    bufBounce.loadFromFile("data/bounce.wav");
    sndBounce.setBuffer(bufBounce);

    bufSmash.loadFromFile("data/smash.wav");
    sndSmash.setBuffer(bufSmash);

    bufHit.loadFromFile("data/hit.wav");
    sndHit.setBuffer(bufHit);


    currentlyPlaying = 0;
    
    musMenu.openFromFile("data/testmusic.ogg");
    musMenu.setLoop(true);

    musGameA.openFromFile("data/LMR.ogg");
    musGameA.setLoop(true);
}

void Audio::stop_all_music() {
    musMenu.stop();
    musGameA.stop();
}

void Audio::update_all_music_volume() {
    musMenu.setVolume((float)options.musicVolume * 100.f);
    musGameA.setVolume((float)options.musicVolume * 100.f);
}

void Audio::on_notify(Event event, void *data) {
    if (event == EVENT_HIT_WATER) {
        sndSplash.setVolume((float)options.sfxVolume * 100.f);
        sndSplash.play();
    }

    if (event == EVENT_BOUNCE_DOOR) {
        sndBounce.setVolume((float)options.sfxVolume * 100.f);
        sndBounce.play();
    }

    if (event == EVENT_SMASH_DOOR) {
        sndSmash.setVolume((float)options.sfxVolume * 100.f);
        sndSmash.play();
    }

    if (event == EVENT_BALL_HIT_SOLID) {
        if (clkHit.getElapsedTime().asSeconds() > .1f) {
            sndHit.setVolume((float)options.sfxVolume * 100.f);
            sndHit.play();
            clkHit.restart();
        }
    }

    if (event == EVENT_CHANGE_MUSIC_VOLUME) {
        std::cout << "Update Vol\n";
        update_all_music_volume();
    }
    
    if (event == EVENT_ENTER_MENU && currentlyPlaying != 1) {
        /* When we enter a menu and we are not already playing the menu music */
        stop_all_music();
        update_all_music_volume();
        currentlyPlaying = 1;
        musMenu.play();
    }

    if (event == EVENT_PLAY_TRACK_A && currentlyPlaying != 2) {
        /* When we enter the game and we are not already playing this */
        stop_all_music();
        update_all_music_volume();
        currentlyPlaying = 2;
        musGameA.play();
    }
}
