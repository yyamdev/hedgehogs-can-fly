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
    EVENT_DUDE_DIE, // int* pointer to dude number
    EVENT_PLAYER_WINS, // NULL
    EVENT_AI_WINS, // NULL
    EVENT_WIND_CHANGE, // sf::Vector2f* pointer to wind vector
    EVENT_DUDE_WEAPON_COOLDOWN_CHANGE, // EntityDude* pointer to dude
    EVENT_LIVES_CHANGE, // int *array[2] player number, lives
    EVENT_TERRAIN_CHANGE, // NULL
    EVENT_BALL_REST_POS, // sf::Vector2f* pointer to new rest position
    EVENT_BALL_START_MOVING, // NULL
    EVENT_PRESS_SPACE, // NULL
    EVENT_HIT_WATER, // NULL
    EVENT_BOUNCE_DOOR, // NULL
    EVENT_SMASH_DOOR, // NULL
    EVENT_BALL_CHANGE_CAN_MOVE, // bool* pointer to canMove?
    EVENT_TNT_EXPLODE, // sf::Vector3f* pointer to explosion position. z = explosion radius
    EVENT_TNT_BEEP, // NULL
};