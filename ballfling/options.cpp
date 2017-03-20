#include "options.h"

namespace options {
    void save() {
    }

    void load() {
        // TODO -> Load from file.
        musicVolume = .3f;
        sfxVolume = .7f;
    }

    float musicVolume;
    float sfxVolume;
};