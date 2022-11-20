
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
    int id;
    double invincible_timer;
    bool can_kick;
    bool can_teleport;
    bool can_survive;
    bool is_dead;
    bool is_moving;
    bool is_bombing;
    bool is_powerup;
    Direction direction;
    Sprite sprite;
    SDL_Color powerup_color;
} Player;

typedef struct
{
    Coords coords;
    int timer;
    int spread_timer;
    int range;
    int damage;
    int spreadCoordsCount;
    int animation_frame_count;
    int current_frame;
    double animation_timer;
    double animation_frame_delay;
    double move_timer;
    double move_delay;
    bool can_move;
    bool is_exploding;
    bool is_dead;
    bool is_moving;
    bool is_active;
    Direction direction;
    Sprite sprite;
    Player *owner;
    Coords *spreadCoords;
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
    Map map;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    Uint64 last_frame_time, current_frame_time;
    bool is_running;
    double delta_time;
    double timer;
    int player_count;
    int main_player_id;
    int bomb_count;
    int powerup_count;
    Player players[4];
    Powerup powerups[MAX_NUMBER_OF_POWERUPS];
    Bomb bombs[MAX_NUMBER_OF_BOMBS];
} Game;

#endif