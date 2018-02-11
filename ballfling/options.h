#pragma once

/*
 * Options that are saved externally in a file.
 */

#include <string>

class Options {
public:
    Options();
    void load(std::string filename);
    void save(std::string filename);

    double musicVolume;
    double sfxVolume;
    int trail;
    int power;
    int fullscreen;
    int edges;
};

extern Options options;
