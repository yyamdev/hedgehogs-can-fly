#pragma once

// list of all possible events to listen for
// data type of the data pointer for on_notify is specified in comments

enum Event {
    EVENT_PLAYER_START_DRAG, // sf::Vector2f* start position
    EVENT_PLAYER_END_DRAG, // NULL
    EVENT_TERRAIN_CHANGE, // sf::Rect<unsigned int>* pointer to rect of reigion to update
    EVENT_BALL_REST_POS, // sf::Vector2f* pointer to new rest position
    EVENT_BALL_MOVE, // sf::Vector2f* pointer to new position
    EVENT_HIT_WATER, // NULL
    EVENT_BOUNCE_DOOR, // NULL
    EVENT_SMASH_DOOR, // NULL
    EVENT_BALL_CHANGE_CAN_FLING, // bool* pointer to canFling
    EVENT_BALL_CHANGE_CAN_NUDGE, // bool* pointer to canNudge
    EVENT_NEW_WORLD_GRAVITY, // NULL
    EVENT_BALL_HIT_FINISH, // NULL
    EVENT_BALL_HIT_SOLID, // NULL
};