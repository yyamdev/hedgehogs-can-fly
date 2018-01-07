#pragma once

/*
 * Save game functionality
 */

bool savegame_is_level_unlocked(int lvl);
int savegame_get_highest_unlocked();
bool savegame_level_unlock(int lvl);
void savegame_load();
void savegame_reset();
