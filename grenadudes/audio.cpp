#include "audio.h"
#include <iostream>

Audio::Audio() {
    bufSplash.loadFromFile("data/splash.wav");
    sndSplash.setBuffer(bufSplash);
}

void Audio::on_notify(Event event, void *data) {
    if (event == EVENT_HIT_WATER) {
        sndSplash.play();
    }
}