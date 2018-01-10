#include <iostream>
#include "libconfig.h"
#include "options.h"
#include "build_options.h"

Options options;

const char* config_string_music = "music_vol";
const char* config_string_sfx   = "sfx_vol";
const char* config_string_trail = "trail";

Options::Options()
{
    load(CONFIG_FILENAME);
}

void Options::load(std::string filename)
{
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

    config_lookup_float(&c, config_string_music, &musicVolume);
    config_lookup_float(&c, config_string_sfx, &sfxVolume);
    config_lookup_bool (&c, config_string_trail, &trail);
    config_destroy(&c);
}

void Options::save(std::string filename)
{
    config_t c;
    config_setting_t *setting;
    config_init(&c);

    setting = config_setting_add(c.root, config_string_music, CONFIG_TYPE_FLOAT);
    config_setting_set_float(setting, musicVolume);

    setting = config_setting_add(c.root, config_string_sfx, CONFIG_TYPE_FLOAT);
    config_setting_set_float(setting, sfxVolume);

    setting = config_setting_add(c.root, config_string_trail, CONFIG_TYPE_BOOL);
    config_setting_set_bool(setting, trail);

    config_write_file(&c, filename.c_str());
    config_destroy(&c);
}
