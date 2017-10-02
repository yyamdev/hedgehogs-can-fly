#pragma once

/*
 * Save game functionality
 */

// Returns t/f depending on if 'lvl' is unlocked
bool savegame_is_level_unlocked(int lvl);

// Returns the furthest level unlocked
int savegame_get_highest_unlocked();

// Make sure all levels are unlocked until 'lvl'
bool savegame_level_unlock(int lvl);

// Re-load save data from file
void savegame_load();

// Reset save game data
void savegame_reset();
