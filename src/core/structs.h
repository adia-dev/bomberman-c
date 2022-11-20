
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

// Powerup
typedef struct
{
    Coords coords;
    PowerupType type;
    double timer;
    double lifetime;
    bool is_active;
    Sprite sprite;
} Powerup;
typedef struct
{
    bool is_running;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    double delta_time;
    Uint64 last_frame_time, current_frame_time;
    Map map;
    Player players[4];
    int player_count;
    int main_player_id;
    Bomb bombs[MAX_NUMBER_OF_BOMBS];
    int bomb_count;
    Powerup powerups[MAX_NUMBER_OF_POWERUPS];
    int powerup_count;
} Game;

#endif