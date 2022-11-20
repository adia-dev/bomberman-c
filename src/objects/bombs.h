#ifndef _BOMBS_H_
#define _BOMBS_H_

#include "powerup.h"

// Bombs functions
void init_bombs(Game *game);
void init_bomb(Bomb *bomb, Player *owner);
void add_bomb(Game *game, Player *owner);
bool place_bomb(Game *game, Player *owner);
bool can_place_bomb(Game *game, SDL_Rect *rect);
void update_bombs(Game *game);
void update_bomb(Game *game, Bomb *bomb);
void draw_bombs(Game *game);
void draw_bomb(Game *game, Bomb *bomb);
void explode_bomb(Game *game, Bomb *bomb);
bool add_explosion(Game *game, Bomb *bomb, Direction direction, int col, int row, int range);
void handle_explosion_direction(Game *game, Bomb *bomb, Direction direction, int col, int row, int range);
void handle_player_explosion(Game *game, Bomb *bomb, int col, int row);
bool drop_powerup(Game *game, int col, int row);
void spread_explosion(Game *game, Bomb *bomb, int col, int row);
void free_bomb(Bomb *bomb);

#endif