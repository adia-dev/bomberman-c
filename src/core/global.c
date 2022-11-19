#include "global.h"

Coords playerInitialCoords[4] = {
    {1, 1},
    {-1, 1},
    {1, -1},
    {-1, -1}};

SDL_Rect tileRects[] = {
    {0, 5, 1, 1},  // GRASS
    {3, 3, 1, 1},  // WALL_UNBREAKABLE
    {4, 3, 1, 1},  // WALL_BREAKABLE
    {11, 3, 1, 1}, // DOOR
    {0, 3, 1, 1},  // BOMB
    {2, 6, 1, 1},  // EXPLOSION_CENTER
    {2, 4, 1, 1},  // EXPLOSION_UP
    {2, 5, 1, 1},  // EXPLOSION_VERTICAL
    {2, 8, 1, 1},  // EXPLOSION_DOWN
    {0, 6, 1, 1},  // EXPLOSION_LEFT
    {1, 6, 1, 1},  // EXPLOSION_HORIZONTAL
    {4, 6, 1, 1},  // EXPLOSION_RIGHT
    {0, 0, 1, 1},  // PLAYER_LEFT
    {0, 1, 1, 1},  // PLAYER_RIGHT
    {3, 1, 1, 1},  // PLAYER_UP
    {3, 0, 1, 1},  // PLAYER_DOWN
    {0, 2, 1, 1},  // PLAYER_DEAD
    {0, 14, 1, 1}, // POWERUP

};