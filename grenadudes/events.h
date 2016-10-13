#pragma once

// list of all possible events to listen for
// data type of the data pointer for on_notify is specified in comments

enum Event {
    EVENT_PLAYER_START_DRAG, // sf::Vector2f start position
    EVENT_PLAYER_END_DRAG, // NULL
    EVENT_ENTITY_HP_CHANGE, // int new hp
    EVENT_DUDE_HP_CHANGE, // EntityDude* pointer to dude
    EVENT_GRENADE_EXPLODE, // Entity* pointer to grenade
    EVENT_DUDE_COOLDOWN_CHANGE, // EntityDude* pointer to dude
    EVENT_AI_STATE_CHANGE, // AiState* pointer to state
    EVENT_ENTITY_DIE, // Entity * pointer to entity
    EVENT_DUDE_DIE // int* pointer to dude number
};