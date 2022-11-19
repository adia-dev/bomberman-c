#ifndef _ENUMS_H_
#define _ENUMS_H_

#include "conf.h"

typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    COUNT
} Direction;

enum Tile
{
    GRASS,                // ' '
    WALL_UNBREAKABLE,     // 'w'
    WALL_BREAKABLE,       // '-'
    DOOR,                 // 'D'
    BOMB,                 // 'B'
    EXPLOSION_CENTER,     // 'x'
    EXPLOSION_UP,         // '^'
    EXPLOSION_VERTICAL,   // '|'
    EXPLOSION_DOWN,       // 'v'
    EXPLOSION_LEFT,       // '<'
    EXPLOSION_HORIZONTAL, // '-'
    EXPLOSION_RIGHT,      // '>'
    PLAYER_LEFT,          // 'L'
    PLAYER_RIGHT,         // 'R'
    PLAYER_UP,            // 'U'
    PLAYER_DOWN,          // 'D',
    PLAYER_DEAD,          // 'X'
    POWERUP,              // '?'
    TILE_COUNT
};

#endif