#include "options.h"
#include "libconfig.h"
#include <iostream>
#include "build_options.h"

Options options; // global options

Options::Options() {
    load(CONFIG_FILENAME);
}

void Options::load(std::string filename) {
    config_t c;
    config_init(&c);
    if (!config_read_file(&c, filename.c_str())) {
        // No config file, use default values:
        config_destroy(&c);
        musicVolume = .5f;
        sfxVolume = .5f;
        trail = true;
        return;
    }
    config_lookup_float(&c, CONFIG_MUSIC, &musicVolume);
    config_lookup_float(&c, CONFIG_SFX, &sfxVolume);
    config_lookup_bool (&c, CONFIG_TRAIL, &trail);
    config_destroy(&c);
}

void Options::save(std::string filename) {
    config_t c;
    config_setting_t *setting;
    config_init(&c);

    setting = config_setting_add(c.root, CONFIG_MUSIC, CONFIG_TYPE_FLOAT);
    config_setting_set_float(setting, musicVolume);

    setting = config_setting_add(c.root, CONFIG_SFX, CONFIG_TYPE_FLOAT);
    config_setting_set_float(setting, sfxVolume);

    setting = config_setting_add(c.root, CONFIG_TRAIL, CONFIG_TYPE_BOOL);
    config_setting_set_bool(setting, trail);

    config_write_file(&c, filename.c_str());
    config_destroy(&c);
}
