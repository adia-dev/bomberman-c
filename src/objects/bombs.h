#ifndef _BOMBS_H_
#define _BOMBS_H_

#include "../map/map.h"

// Bombs functions
void init_bombs(Game *game);
void init_bomb(Bomb *bomb, Player *owner);
void add_bomb(Game *game, Player *owner);
bool place_bomb(Game *game, Player *owner);
bool can_place_bomb(Game *game, SDL_Rect *rect);
void update_bombs(Game *game);
void update_bomb(Game *game, Bomb *bomb);
void explode_bomb(Game *game, Bomb *bomb);
bool add_explosion(Game *game, Bomb *bomb, Direction direction, int col, int row);
void free_bomb(Bomb *bomb);

#endif