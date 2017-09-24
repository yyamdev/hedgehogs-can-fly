#pragma once

// global game options

#include <string>

// config file options
#define CONFIG_MUSIC    "music_vol"
#define CONFIG_SFX      "sfx_vol"
#define CONFIG_TRAIL    "trail"

class Options {
public:
    Options();
    void load(std::string filename);
    void save(std::string filename);

    double musicVolume;
    double sfxVolume;
    int trail;
};

extern Options options; // global options
