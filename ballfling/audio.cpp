#include "audio.h"
#include <iostream>
#include "options.h"

Audio::Audio() {
    bufSplash.loadFromFile("data/splash.wav");
    sndSplash.setBuffer(bufSplash);

    bufBounce.loadFromFile("data/bounce.wav");
    sndBounce.setBuffer(bufBounce);

    bufSmash.loadFromFile("data/smash.wav");
    sndSmash.setBuffer(bufSmash);

    bufExplode.loadFromFile("data/asplode.ogg");
    sndExplode.setBuffer(bufExplode);

    bufTbeep.loadFromFile("data/tntbeep.wav");
    sndTbeep.setBuffer(bufTbeep);

    bufHit.loadFromFile("data/hit.wav");
    sndHit.setBuffer(bufHit);
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

    if (event == EVENT_TNT_EXPLODE) {
        sndExplode.setVolume((float)options.sfxVolume * 100.f);
        sndExplode.play();
    }

    if (event == EVENT_TNT_BEEP) {
        sndTbeep.setVolume((float)options.sfxVolume * 100.f);
        sndTbeep.play();
    }

    if (event == EVENT_BALL_HIT_SOLID) {
        if (clkHit.getElapsedTime().asSeconds() > .1f) {
            sndHit.setVolume((float)options.sfxVolume * 100.f);
            sndHit.play();
            clkHit.restart();
        }
    }
}