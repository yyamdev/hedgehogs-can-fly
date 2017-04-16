#pragma once

// list of all possible events to listen for
// data type of the data pointer for on_notify is specified in comments

enum Event {
    EVENT_PLAYER_START_DRAG, // sf::Vector2f* start position
    EVENT_PLAYER_END_DRAG, // NULL
    EVENT_ENTITY_HP_CHANGE, // int new hp
    EVENT_DUDE_HP_CHANGE, // EntityDude* pointer to dude
    EVENT_GRENADE_EXPLODE, // Entity* pointer to grenade
    EVENT_DUDE_COOLDOWN_CHANGE, // EntityDude* pointer to dude
    EVENT_AI_STATE_CHANGE, // AiState* pointer to state
    EVENT_ENTITY_DIE, // Entity * pointer to entity
    EVENT_DUDE_DIE, // int* pointer to dude number
    EVENT_PLAYER_WINS, // NULL
    EVENT_AI_WINS, // NULL
    EVENT_WIND_CHANGE, // sf::Vector2f* pointer to wind vector
    EVENT_DUDE_WEAPON_COOLDOWN_CHANGE, // EntityDude* pointer to dude
    EVENT_LIVES_CHANGE, // int *array[2] player number, lives
    EVENT_TERRAIN_CHANGE, // sf::Rect<unsigned int>* pointer to rect of reigion to update
    EVENT_BALL_REST_POS, // sf::Vector2f* pointer to new rest position
    EVENT_BALL_MOVE, // sf::Vector2f* pointer to new position
    EVENT_PRESS_SPACE, // NULL
    EVENT_HIT_WATER, // NULL
    EVENT_BOUNCE_DOOR, // NULL
    EVENT_SMASH_DOOR, // NULL
    EVENT_BALL_CHANGE_CAN_FLING, // bool* pointer to canFling
    EVENT_BALL_CHANGE_CAN_NUDGE, // bool* pointer to canNudge
    EVENT_TNT_EXPLODE, // sf::Vector3f* pointer to explosion position. z = explosion radius
    EVENT_TNT_BEEP, // NULL
    EVENT_NEW_WORLD_GRAVITY, // NULL
    EVENT_BALL_HIT_FINISH, // NULL
    EVENT_BALL_HIT_SOLID, // NULL
};