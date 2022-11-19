
#ifndef _STRUCTS_H_
#define _STRUCTS_H_
#include "enums.h"

typedef struct
{
    int x;
    int y;
} Coords;

typedef struct
{
    int width;
    int height;
    char **data;
} Map;

typedef struct
{
    SDL_Texture *texture;
    SDL_Rect src_rect;
    SDL_Rect dst_rect;
} Sprite;

typedef struct
{
    Coords coords;
    int bomb_range;
    int bomb_count;
    int bomb_timer;
    int explosion_timer;
    int speed;
    int lives;
    int score;
    int powerup;
    int powerup_timer;
    bool is_dead;
    bool is_moving;
    bool is_bombing;
    bool is_powerup;
    Direction direction;
    Sprite sprite;
    int id;
} Player;

typedef struct
{
    Coords coords;
    int timer;
    int spread_timer;
    int range;
    int damage;
    bool can_move;
    bool is_exploding;
    bool is_dead;
    bool is_moving;
    bool is_active;
    Sprite sprite;
    Player *owner;
    Coords *spreadCoords;
    int spreadCoordsCount;
} Bomb;

typedef struct
{
    bool is_running;
    SDL_Window *window;
    SDL_Renderer *renderer;
    Map map;
    Player players[4];
    int player_count;
    SDL_Texture *texture;
    int bomb_count;
    double delta_time;
    Bomb bombs[MAX_NUMBER_OF_BOMBS];
    Uint64 last_frame_time, current_frame_time;
} Game;

#endif