#pragma once

// list of all possible events to listen for
// data type of the data pointer for on_notify is specified in comments

enum Event {
    EVENT_PLAYER_START_DRAG, // sf::Vector2f* start position
    EVENT_PLAYER_END_DRAG, // NULL
    EVENT_TERRAIN_CHANGE, // sf::Rect<unsigned int>* pointer to rect of reigion to update
    EVENT_BALL_REST_POS, // sf::Vector2f* pointer to new rest position
    EVENT_BALL_MOVE, // sf::Vector2f* pointer to new position
    EVENT_BOUNCE_DOOR, // NULL
    EVENT_SMASH_DOOR, // NULL
    EVENT_BALL_CHANGE_CAN_FLING, // bool* pointer to canFling
    EVENT_BALL_CHANGE_CAN_NUDGE, // bool* pointer to canNudge
    EVENT_NEW_WORLD_GRAVITY, // NULL
    EVENT_PLAY_RESUME, // NULL
    EVENT_PLAY_PAUSE, // NULL
    EVENT_BALL_FLING, // NULL

    EVENT_CHANGE_MUSIC_VOLUME, // NULL
    EVENT_ENTER_MENU, // NULL
    EVENT_ENTER_GAME, // NULL

    EVENT_BALL_HIT_WATER, // NULL
    EVENT_BALL_HIT_TRIGGER, // NULL
    EVENT_BALL_HIT_FINISH, // NULL
    EVENT_BALL_HIT_SOLID, // NULL
    EVENT_BALL_HIT_BOUNCY, // NULL
    EVENT_BALL_HIT_STICKY, // NULL
    EVENT_BALL_HIT_SANDY, // NULL

    EVENT_LEVEL_START, // NULL

    EVENT_MENU_CLICK, // NULL
    EVENT_MENU_LOCKED, // NULL

    EVENT_ENTER_SPLASH, // NULL
};
