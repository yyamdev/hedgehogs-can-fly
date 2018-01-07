#include "assert.h"
#include "audio.h"
#include <iostream>
#include "options.h"
#include "subject.h"

enum Sfx {
 // SFX_XXXXXXXX
    SFX_HITSOLID,
    SFX_HITBOUNC,
    SFX_MENUCLIC,
    SFX_LVLSTART,
    SFX_HITSANDY,
    SFX_MENULOCK,
    SFX_HITSTCKY,
    SFX_DIEWATER,
    SFX_SPLASHSC,
    SFX_FIREWORK,
    SFX_BALLDRAG,
    SFX_BALLFLNG,
    SFX_ENTPAUSE,
    SFX_LVFINISH,
    SFX_COUNT,
};

static const char * names[SFX_COUNT] = {
    /* SFX_HITSOLID */  "data/sfx/hit_solid.wav",
    /* SFX_HITCOUNC */  "data/sfx/hit_bouncy.wav",
    /* SFX_MENUCLIC */  "data/sfx/menu_click.wav",
    /* SFX_LVLSTART */  "data/sfx/level_start.wav",
    /* SFX_HITSANDY */  "data/sfx/hit_sand.wav",
    /* SFX_MENULOCK */  "data/sfx/menu_locked.wav",
    /* SFX_HITSTCKY */  "data/sfx/hit_sticky.wav",
    /* SFX_DIEWATER */  "data/sfx/die_water.wav",
    /* SFX_SPLASHSC */  "data/sfx/splash.wav",
    /* SFX_FIREWORK */  "data/sfx/firework.wav",
    /* SFX_BALLDRAG */  "data/sfx/ball_drag.wav",
    /* SFX_BALLFLNG */  "data/sfx/ball_fling.wav",
    /* SFX_ENTPAUSE */  "data/sfx/pause.wav",
    /* SFX_LVFINISH */  "data/sfx/level_complete.wav",
};
static sf::SoundBuffer buf[SFX_COUNT];
static sf::Sound       snd[SFX_COUNT];

void sfx_play(Sfx sfx)
{
    snd[sfx].setVolume((float)options.sfxVolume * 100.f);
    snd[sfx].play();
}

void Audio::reload_sfx()
{
    for (int i = 0; i < SFX_COUNT; ++i) {
        buf[i].loadFromFile(names[i]);
        snd[i].setBuffer(buf[i]);
        snd[i].setLoop(false);
    }
}

Audio::Audio()
{
    Subject::add_observer(this);

    reload_sfx();

    currentlyPlaying = MUSIC_TRACK_COUNT;
    wasCurrentlyPlaying = MUSIC_TRACK_A;

    musMenu.openFromFile("data/music/menu.ogg");
    musMenu.setLoop(true);

    musGameA.openFromFile("data/music/a.ogg");
    musGameA.setLoop(false);

    musGameB.openFromFile("data/music/b.ogg");
    musGameB.setLoop(false);

    musGameC.openFromFile("data/music/c.ogg");
    musGameC.setLoop(false);
}

void Audio::stop_all_music()
{
    musMenu.stop();
    musGameA.stop();
    musGameB.stop();
    musGameC.stop();
}

void Audio::update_all_music_volume()
{
    musMenu.setVolume((float)options.musicVolume * 100.f);
    musGameA.setVolume((float)options.musicVolume * 100.f);
    musGameB.setVolume((float)options.musicVolume * 100.f);
    musGameC.setVolume((float)options.musicVolume * 100.f);
}

void Audio::on_notify(Event event, void *data)
{
    // Sfx

    if (event == EVENT_BALL_HIT_SOLID) {
        if (clkHit.getElapsedTime().asSeconds() > .1f) {
            sfx_play(SFX_HITSOLID);
            clkHit.restart();
        }
    }

    if (event == EVENT_BALL_HIT_SANDY) {
        if (clkHitSandy.getElapsedTime().asSeconds() > .25f) {
            sfx_play(SFX_HITSANDY);
            clkHitSandy.restart();
        }
    }

    if (event == EVENT_BALL_HIT_BOUNCY)    sfx_play(SFX_HITBOUNC);
    if (event == EVENT_MENU_CLICK)         sfx_play(SFX_MENUCLIC);
    if (event == EVENT_LEVEL_START)        sfx_play(SFX_LVLSTART);
    if (event == EVENT_MENU_LOCKED)        sfx_play(SFX_MENULOCK);
    if (event == EVENT_BALL_HIT_STICKY)    sfx_play(SFX_HITSTCKY);
    if (event == EVENT_BALL_HIT_WATER)     sfx_play(SFX_DIEWATER);
    if (event == EVENT_ENTER_SPLASH)       sfx_play(SFX_SPLASHSC);
    if (event == EVENT_FIREWORK)           sfx_play(SFX_FIREWORK);
    if (event == EVENT_START_DRAG)  sfx_play(SFX_BALLDRAG);
    if (event == EVENT_END_DRAG)    sfx_play(SFX_BALLFLNG);
    if (event == EVENT_ENTER_PAUSE_SCREEN) sfx_play(SFX_ENTPAUSE);
    if (event == EVENT_LEVEL_COMPLETE)     sfx_play(SFX_LVFINISH);

    if (event == EVENT_EXIT_SPLASH)        snd[SFX_SPLASHSC].stop();

    // Music

    if (event == EVENT_ENTER_END_SCREEN) {
        stop_all_music();
        currentlyPlaying = MUSIC_TRACK_COUNT;
    }

    if (event == EVENT_CHANGE_MUSIC_VOLUME) {
        update_all_music_volume();
    }

    if (event == EVENT_LEVEL_COMPLETE) {
        track_to_music(currentlyPlaying)->pause();
    }

    if (event == EVENT_ENTER_MENU && currentlyPlaying != MUSIC_TRACK_MENU) {
        // When we enter a menu and we are not already playing the menu music

        if (currentlyPlaying != MUSIC_TRACK_COUNT &&
            currentlyPlaying > MUSIC_TRACK_MENU)
        {
            /* Transition game -> menu
             * On resume, start playing the NEXT game song. */
            go_to_next_game_track();
            wasCurrentlyPlaying = currentlyPlaying;
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
            // Transition menu -> game
            currentlyPlaying = wasCurrentlyPlaying;
            stop_all_music();
            update_all_music_volume();
            track_to_music(currentlyPlaying)->play();
        }
    }
}

sf::Music * Audio::track_to_music(track track_enum)
{
    switch (track_enum) {
    case MUSIC_TRACK_MENU:
        return &musMenu;
    case MUSIC_TRACK_A:
        return &musGameA;
    case MUSIC_TRACK_B:
        return &musGameB;
    case MUSIC_TRACK_C:
        return &musGameC;
    }

    assert(false);
    return NULL;
}

void Audio::go_to_next_game_track()
{
    currentlyPlaying = (track)((int)currentlyPlaying + 1);
    if (currentlyPlaying == MUSIC_TRACK_COUNT)
        currentlyPlaying = MUSIC_TRACK_A; // loop back to track A
}

void Audio::tick_game_music()
{
    if (currentlyPlaying != MUSIC_TRACK_COUNT &&
        currentlyPlaying > MUSIC_TRACK_MENU)
    {
        /* Currently playing a game music */
        sf::Music *current = track_to_music(currentlyPlaying);

        if (current->getStatus() == sf::SoundSource::Status::Stopped) {
            go_to_next_game_track();
            stop_all_music();
            update_all_music_volume();
            track_to_music(currentlyPlaying)->play();
        }
    }
}
