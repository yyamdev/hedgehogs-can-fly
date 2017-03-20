#pragma once

// global game options

#include <string>

class Options {
public:
    Options();
    void load();
    void save();

    float musicVolume;
    float sfxVolume;
};

extern Options options; // global options