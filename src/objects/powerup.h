#ifndef _POWERUP_H_
#define _POWERUP_H_

#include "../player/player.h"

void init_powerups(Game *game);
void init_powerup(Powerup *powerup);
void update_powerups(Game *game);
void update_powerup(Game *game, Powerup *powerup);
void draw_powerup(Game *game, Powerup *powerup);
void draw_powerups(Game *game);
bool spawn_powerup(Game *game, int x, int y);
SDL_Color get_powerup_color(Powerup *powerup);
void spawn_powerup_randomly(Game *game);

#endif