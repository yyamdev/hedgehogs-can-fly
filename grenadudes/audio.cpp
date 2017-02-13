#include "audio.h"
#include <iostream>

Audio::Audio() {
    bufSplash.loadFromFile("data/splash.wav");
    sndSplash.setBuffer(bufSplash);

    bufBounce.loadFromFile("data/bounce.wav");
    sndBounce.setBuffer(bufBounce);
}

void Audio::on_notify(Event event, void *data) {
    if (event == EVENT_HIT_WATER) {
        sndSplash.play();
    }

    if (event == EVENT_BOUNCE_DOOR) {
        sndBounce.play();
    }
}