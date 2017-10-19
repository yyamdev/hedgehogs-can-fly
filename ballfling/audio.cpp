#include "assert.h"
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


    currentlyPlaying = MUSIC_TRACK_COUNT;
    wasCurrentlyPlaying = MUSIC_TRACK_A;
    
    musMenu.openFromFile("data/testmusic.ogg");
    musMenu.setLoop(true);

    musGameA.openFromFile("data/Kings Gift.ogg");
    musGameA.setLoop(false); // gameplay tracks are switched out when finished

    musGameB.openFromFile("data/LMR.ogg");
    musGameB.setLoop(false);
}

void Audio::stop_all_music() {
    musMenu.stop();
    musGameA.stop();
    musGameB.stop();
}

void Audio::update_all_music_volume() {
    musMenu.setVolume((float)options.musicVolume * 100.f);
    musGameA.setVolume((float)options.musicVolume * 100.f);
    musGameB.setVolume((float)options.musicVolume * 100.f);
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
        update_all_music_volume();
    }
    
    if (event == EVENT_ENTER_MENU && currentlyPlaying != MUSIC_TRACK_MENU) {
        /* When we enter a menu and we are not already playing the menu music */

        if (currentlyPlaying != MUSIC_TRACK_COUNT &&
            currentlyPlaying > MUSIC_TRACK_MENU)
        {
            /* Just came from a game, save the track */
            wasCurrentlyPlaying = currentlyPlaying;
            std::cout << "save track\n";
        }
        
        stop_all_music();
        update_all_music_volume();
        currentlyPlaying = MUSIC_TRACK_MENU;
        musMenu.play();
    }

    if (event == EVENT_ENTER_GAME) {
        if (currentlyPlaying == MUSIC_TRACK_MENU ||
            currentlyPlaying == MUSIC_TRACK_COUNT)
        {
            /* Just entered the game */
            currentlyPlaying = wasCurrentlyPlaying;
            stop_all_music();
            update_all_music_volume();
            track_to_music(currentlyPlaying)->play();
        }

        else if (currentlyPlaying != MUSIC_TRACK_COUNT &&
                 currentlyPlaying > MUSIC_TRACK_MENU)
        {
            /* Entered the game when already inside the game */
            // NOTE: Delete this unless needed
            std::cout << "test\n";
        }
    }
}

sf::Music * Audio::track_to_music(track track_enum) {
    if (track_enum == MUSIC_TRACK_MENU)
        return &musMenu;
    if (track_enum == MUSIC_TRACK_A)
        return &musGameA;
    if (track_enum == MUSIC_TRACK_B)
        return &musGameB;
    else {
        assert(false); // Your enum is probably screweded up
        return NULL;
    }
}

void Audio::tick_game_music() {
    if (currentlyPlaying != MUSIC_TRACK_COUNT &&
        currentlyPlaying > MUSIC_TRACK_MENU)
    {
        /* Currently playing a game music */
        sf::Music *current = track_to_music(currentlyPlaying);

        if (current->getStatus() == sf::SoundSource::Status::Stopped) {
            std::cout << "Next Track\n";
            currentlyPlaying = (track)((int)currentlyPlaying + 1);
            if (currentlyPlaying == MUSIC_TRACK_COUNT)
                currentlyPlaying = MUSIC_TRACK_A; // loop back to track A
            stop_all_music();
            update_all_music_volume();
            track_to_music(currentlyPlaying)->play();
        }
    }
}
