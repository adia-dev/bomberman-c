#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "../map/map.h"

// Player functions
void init_player(Game *game, Player *player, int x, int y);
void add_player(Game *game);
void draw_players(Game *game);
void draw_player(Game *game, Player *player);
void move_player(Game *game, Player *player, Direction direction);
Powerup *collide_with_powerup(Game *game, int x, int y);
bool consume_powerup(Game *game, Player *player, Powerup *powerup);
bool is_valid_move(Game *game, SDL_Rect *rect);
void update_player_rect(Player *player);
void take_damage(Game *game, Player *player, int damage);

bool kill_player(Game *game, Player *player);

#endif