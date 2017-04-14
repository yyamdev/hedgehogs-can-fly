#include "animation.h"

Animation::Animation() {
    Animation(1, 60);
}

Animation::Animation(int frames, int fps) {
    this->frames = frames;
    this->fps = fps;
    clock.restart();
}

int Animation::get_frame() {
    return (int)(clock.getElapsedTime().asSeconds() / (1.f / fps)) % frames;
}