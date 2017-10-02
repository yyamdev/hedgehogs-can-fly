#include "save.h"
#include "libconfig.h"
#include <stdio.h>

static int prog = 1;
static bool loaded = false;
static char *filename = "data/prog";
static char *entry = "prog";

void savegame_save();

bool savegame_is_level_unlocked(int lvl)
{
    if (!loaded) savegame_load();
    return lvl <= prog;
}

int savegame_get_highest_unlocked()
{
    if (!loaded) savegame_load();
    return prog;
}

bool savegame_level_unlock(int lvl)
{
    if (lvl > prog) {
        prog = lvl;
        savegame_save();
        return true; // Level was unlocked
    }
    return false; // Level was already unlocked
}

void savegame_reset()
{
    prog = 1;
    savegame_save();
}

void savegame_save()
{
    config_t c;
    config_setting_t *setting;
    config_init(&c);

    setting = config_setting_add(c.root, entry, CONFIG_TYPE_INT);
    config_setting_set_int(setting, prog);
    
    config_write_file(&c, filename);
    config_destroy(&c);
}

void savegame_load()
{
    config_t c;
    config_init(&c);
    if (config_read_file(&c, filename)) {
        config_lookup_int(&c, entry, &prog);
    }
    config_destroy(&c);
    loaded = true;
}
