#ifndef _MAP_H_
#define _MAP_H_

#include "../core/global.h"

// Map functions
void load_map(Map *map, const char *filename);
void init_map(Map *map);
void draw_map(Game *game);
void draw_tile(Game *game, int tile, int x, int y);
SDL_Rect get_tile_rect(int tile);
char get_player_tile(Player *player);

bool is_tile_player(char tile);
bool is_tile_wall(char tile);
bool is_tile_breakable(char tile);
bool is_tile_bomb(char tile);
bool is_tile_explosion(char tile);
bool is_tile_powerup(char tile);
bool is_tile_empty(char tile);

#endif