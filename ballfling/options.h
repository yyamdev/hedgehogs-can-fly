#pragma once

// global game options

#include <string>

// config file options
#define CONFIG_MUSIC    "music_vol"
#define CONFIG_SFX      "sfx_vol"

class Options {
public:
    Options();
    void load(std::string filename);
    void save(std::string filename);

    double musicVolume;
    double sfxVolume;
};

extern Options options; // global options