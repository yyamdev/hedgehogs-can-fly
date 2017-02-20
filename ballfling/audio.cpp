#include "audio.h"
#include <iostream>

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
}

void Audio::on_notify(Event event, void *data) {
    if (event == EVENT_HIT_WATER) {
        sndSplash.play();
    }

    if (event == EVENT_BOUNCE_DOOR) {
        sndBounce.play();
    }

    if (event == EVENT_SMASH_DOOR) {
        sndSmash.play();
    }

    if (event == EVENT_TNT_EXPLODE) {
        sndExplode.play();
    }

    if (event == EVENT_TNT_BEEP) {
        sndTbeep.play();
    }
}