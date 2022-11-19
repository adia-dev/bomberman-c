#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define TILE_SIZE 16
#define SCALE 4
#define TEXTURE_WIDTH 224
#define TEXTURE_HEIGHT 384
#define MAX_BUFFER_SIZE 4096
#define MAX_MAP_WIDTH 20
#define MAX_MAP_HEIGHT 20
#define MAX_NUMBER_OF_BOMBS 20
#define BOMB_LIFE_TIME 1500
#define BOMB_SPREAD_LIFE_TIME 500

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

typedef struct
{
    int x;
    int y;
} Coords;

Coords playerInitialCoords[4] = {
    {1, 1},
    {-1, 1},
    {1, -1},
    {-1, -1}};

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

Player *current_player = NULL;

// Game functions
void init_game(Game *game);
void init_map(Map *map);
void init_player(Game *game, Player *player, int x, int y);
void load_map(Map *map, const char *filename);
void load_texture(Game *game, const char *filename);
void process_input(Game *game);
void update(Game *game);
void render(Game *game);
void destroy_game(Game *game);

// Map functions
void draw_map(Game *game);
void draw_tile(Game *game, int tile, int x, int y);
SDL_Rect get_tile_rect(int tile);
char get_player_tile(Player *player);

// Player functions
void add_player(Game *game);
void draw_players(Game *game);
void draw_player(Game *game, Player *player);
void move_player(Game *game, Player *player, Direction direction);
bool is_valid_move(Game *game, SDL_Rect *rect);
void update_player_rect(Player *player);
void update_player_texture(Game *game, Direction direction);

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

int main(int argc, char *argv[])
{
    Game game;
    init_game(&game);

    add_player(&game);
    add_player(&game);
    add_player(&game);
    add_player(&game);

    // add_player(&game);

    while (game.is_running)
    {
        process_input(&game);
        update(&game);
        render(&game);
    }

    destroy_game(&game);

    return 0;
}

void init_game(Game *game)
{
    game->is_running = true;
    game->window = NULL;
    game->renderer = NULL;
    game->texture = NULL;
    game->player_count = 0;
    game->bomb_count = 0;
    game->delta_time = 0.0;
    game->last_frame_time = 0;
    game->current_frame_time = SDL_GetPerformanceCounter();

    for (int i = 0; i < TILE_COUNT; i++)
    {
        tileRects[i].x *= TILE_SIZE;
        tileRects[i].y *= TILE_SIZE;
        tileRects[i].w *= TILE_SIZE;
        tileRects[i].h *= TILE_SIZE;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Error initializing SDL.\n");
        exit(1);
    }

    game->window = SDL_CreateWindow("Bomberman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, TEXTURE_WIDTH * SCALE, TEXTURE_HEIGHT * SCALE, 0);

    if (!game->window)
    {
        fprintf(stderr, "Error creating SDL window.\n");
        exit(1);
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);

    if (!game->renderer)
    {
        fprintf(stderr, "Error creating SDL renderer.\n");
        exit(1);
    }

    init_map(&game->map);
    load_map(&game->map, "../src/levels/level_01.txt");
    load_texture(game, "../src/assets/NES_BOMBERMAN.png");

    // resize window the size of a TILE_SIZE and the map width and height
    SDL_SetWindowSize(game->window, game->map.width * TILE_SIZE * SCALE, game->map.height * TILE_SIZE * SCALE);
}

void init_map(Map *map)
{
    map->width = 0;
    map->height = 0;
    map->data = NULL;
}

void init_player(Game *game, Player *player, int x, int y)
{
    player->coords.x = x;
    player->coords.y = y;
    player->bomb_range = 3;
    player->bomb_count = 100;
    player->bomb_timer = 0;
    player->speed = 1;
    player->lives = 1;
    player->score = 0;
    player->powerup = 0;
    player->powerup_timer = 0;
    player->is_dead = false;
    player->is_moving = false;
    player->is_bombing = false;
    player->is_powerup = false;
    player->direction = DOWN;
    player->sprite.texture = NULL;

    SDL_Rect src_rect = {0, 0, TILE_SIZE, TILE_SIZE};
    SDL_Rect dst_rect = {x * TILE_SIZE * SCALE, y * TILE_SIZE * SCALE, TILE_SIZE * SCALE, TILE_SIZE * SCALE};

    player->sprite.src_rect = src_rect;
    player->sprite.dst_rect = dst_rect;

    player->id = game->player_count;

    game->map.data[y][x] = get_player_tile(player);
}

void load_map(Map *map, const char *filename)
{
    char *fullpath = realpath(filename, NULL);
    FILE *file = fopen("/Users/abdoulayedia/Projects/Dev/C/bomberman/src/levels/level_01.txt", "r");
    if (!file)
    {
        fprintf(stderr, "Error opening file.\n");
        exit(1);
    }

    char buffer[MAX_BUFFER_SIZE];
    int row = 0;
    while (fgets(buffer, MAX_BUFFER_SIZE, file))
    {
        if (map->width == 0)
        {
            map->width = strlen(buffer) - 1;
        }

        map->height++;
    }

    map->data = malloc(sizeof(char *) * map->height);
    for (int i = 0; i < map->height; i++)
    {
        map->data[i] = malloc(sizeof(char) * map->width);
    }

    rewind(file);

    while (fgets(buffer, MAX_BUFFER_SIZE, file))
    {
        for (int col = 0; col < map->width; col++)
        {
            map->data[row][col] = buffer[col];
        }
        row++;
    }

    fclose(file);
}

void load_texture(Game *game, const char *filename)
{
    char *fullpath = realpath(filename, NULL);
    SDL_Surface *surface = IMG_Load("/Users/abdoulayedia/Projects/Dev/C/bomberman/src/assets/NES_BOMBERMAN.png");
    if (!surface)
    {
        fprintf(stderr, "Error loading image.\n");
        exit(1);
    }

    game->texture = SDL_CreateTextureFromSurface(game->renderer, surface);

    SDL_FreeSurface(surface);
}

void process_input(Game *game)
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type)
    {
    case SDL_QUIT:
        game->is_running = false;
        break;
    case SDL_KEYDOWN:
        if (current_player != NULL)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                game->is_running = false;
                break;
            case SDLK_LEFT:
                move_player(game, current_player, LEFT);
                break;
            case SDLK_RIGHT:
                move_player(game, current_player, RIGHT);
                break;
            case SDLK_UP:
                move_player(game, current_player, UP);
                break;
            case SDLK_DOWN:
                move_player(game, current_player, DOWN);
                break;
            case SDLK_e:
                if (place_bomb(game, current_player))
                {
                    current_player->bomb_count--;
                }
                else
                {
                    printf("NO BOMB\n");
                }
                break;
            case SDLK_j:
                if (game->player_count > 1)
                {
                    Player *secondPlayer = &game->players[1];
                    move_player(game, secondPlayer, LEFT);
                }
                break;
            case SDLK_l:
                if (game->player_count > 1)
                {
                    Player *secondPlayer = &game->players[1];
                    move_player(game, secondPlayer, RIGHT);
                }
                break;
            case SDLK_i:
                if (game->player_count > 1)
                {
                    Player *secondPlayer = &game->players[1];
                    move_player(game, secondPlayer, UP);
                }
                break;
            case SDLK_k:
                if (game->player_count > 1)
                {
                    Player *secondPlayer = &game->players[1];
                    move_player(game, secondPlayer, DOWN);
                }
                break;
            case SDLK_o:
                if (game->player_count > 1)
                {
                    Player *secondPlayer = &game->players[1];
                    if (place_bomb(game, secondPlayer))
                    {
                        secondPlayer->bomb_count--;
                    }
                    else
                    {
                        printf("NO BOMB\n");
                    }
                }
                break;
            }
            break;
        }
    }
}

void update(Game *game)
{

    // update delta time
    game->last_frame_time = game->current_frame_time;
    game->current_frame_time = SDL_GetPerformanceCounter();

    game->delta_time = (double)((game->current_frame_time - game->last_frame_time) * 1000 / (double)SDL_GetPerformanceFrequency());

    // printf("DELTA TIME(ms): %f\n", game->delta_time);
    update_bombs(game);
}

void render(Game *game)
{
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);

    draw_map(game);
    draw_players(game);

    SDL_RenderPresent(game->renderer);
}

void destroy_game(Game *game)
{

    for (int i = 0; i < game->map.height; i++)
    {
        free(game->map.data[i]);
    }
    free(game->map.data);

    for (int i = 0; i < game->bomb_count; i++)
    {
        free_bomb(&game->bombs[i]);
    }

    SDL_DestroyTexture(game->texture);
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    SDL_Quit();
}

void draw_map(Game *game)
{
    for (int row = 0; row < game->map.height; row++)
    {
        for (int col = 0; col < game->map.width; col++)
        {
            int tile = game->map.data[row][col];
            draw_tile(game, tile, col * TILE_SIZE, row * TILE_SIZE);
        }
    }
}

void draw_tile(Game *game, int tile, int x, int y)
{
    SDL_Rect src_rect = get_tile_rect(tile);

    SDL_Rect dst_rect;
    dst_rect.x = x * SCALE;
    dst_rect.y = y * SCALE;
    dst_rect.w = TILE_SIZE * SCALE;
    dst_rect.h = TILE_SIZE * SCALE;

    SDL_RenderCopy(game->renderer, game->texture, &src_rect, &dst_rect);
}

SDL_Rect get_tile_rect(int tile)
{
    switch (tile)
    {
    case ' ':
        return tileRects[GRASS];
    case 'w':
        return tileRects[WALL_UNBREAKABLE];
    case '.':
        return tileRects[WALL_BREAKABLE];
    case 'd':
        return tileRects[DOOR];
    case 'b':
        return tileRects[BOMB];
    case '^':
        return tileRects[EXPLOSION_UP];
    case 'v':
        return tileRects[EXPLOSION_DOWN];
    case '<':
        return tileRects[EXPLOSION_LEFT];
    case '>':
        return tileRects[EXPLOSION_RIGHT];
    case 'x':
        return tileRects[EXPLOSION_CENTER];
    case '-':
        return tileRects[EXPLOSION_HORIZONTAL];
    case '|':
        return tileRects[EXPLOSION_VERTICAL];
    case '?':
        return tileRects[POWERUP];
    default:
        return tileRects[GRASS];
    }
}

char get_player_tile(Player *player)
{
    switch (player->id)
    {
    case 1:
        return '1';
    case 2:
        return '2';
    case 3:
        return '3';
    case 4:
        return '4';
    default:
        return '0';
    }
}

void add_player(Game *game)
{

    if (game->player_count < 4)
    {
        Player *player = &game->players[game->player_count++];
        int x = playerInitialCoords[game->player_count - 1].x;
        int y = playerInitialCoords[game->player_count - 1].y;

        if (x == -1)
            x = game->map.width - 2;
        if (y == -1)
            y = game->map.height - 2;

        printf("Player %d added at (%d, %d)\n", game->player_count, x, y);

        init_player(game, player, x, y);
        if (current_player == NULL)
        {
            current_player = player;
        }
    }
    else
    {
        printf("Max players reached\n");
    }
}

void draw_players(Game *game)
{
    for (int i = 0; i < game->player_count; i++)
    {
        Player *player = &game->players[i];
        draw_player(game, player);
    }
}

void draw_player(Game *game, Player *player)
{
    update_player_rect(player);

    SDL_RenderCopy(game->renderer, game->texture, &player->sprite.src_rect, &player->sprite.dst_rect);
}

void update_player_rect(Player *player)
{
    switch (player->direction)
    {
    case LEFT:
        player->sprite.src_rect = tileRects[PLAYER_LEFT];
        break;
    case RIGHT:
        player->sprite.src_rect = tileRects[PLAYER_RIGHT];
        break;
    case UP:
        player->sprite.src_rect = tileRects[PLAYER_UP];
        break;
    case DOWN:
        player->sprite.src_rect = tileRects[PLAYER_DOWN];
        break;
    default:
        break;
    }
}

void move_player(Game *game, Player *player, Direction direction)
{
    if (game == NULL || player == NULL)
        return;

    SDL_Rect player_rect = player->sprite.dst_rect;

    int old_x = player_rect.x / (TILE_SIZE * SCALE);
    int old_y = player_rect.y / (TILE_SIZE * SCALE);

    switch (direction)
    {
    case LEFT:
        player_rect.x -= TILE_SIZE * SCALE;
        break;
    case RIGHT:
        player_rect.x += TILE_SIZE * SCALE;
        break;
    case UP:
        player_rect.y -= TILE_SIZE * SCALE;
        break;
    case DOWN:
        player_rect.y += TILE_SIZE * SCALE;
        break;
    default:
        break;
    }

    if (is_valid_move(game, &player_rect))
    {
        player->direction = direction;
        player->sprite.dst_rect = player_rect;
        int x = player_rect.x / (TILE_SIZE * SCALE);
        int y = player_rect.y / (TILE_SIZE * SCALE);
        game->map.data[old_y][old_x] = ' ';
        game->map.data[y][x] = get_player_tile(player);
    }
}

bool is_valid_move(Game *game, SDL_Rect *rect)
{
    int row = rect->y / (TILE_SIZE * SCALE);
    int col = rect->x / (TILE_SIZE * SCALE);

    if (row < 0 || row >= game->map.height || col < 0 || col >= game->map.width)
    {
        return false;
    }

    return game->map.data[row][col] == ' ';
}

// Bombs functions
void init_bombs(Game *game)
{
    for (int i = 0; i < MAX_NUMBER_OF_BOMBS; i++)
    {
        init_bomb(&game->bombs[i], NULL);
    }
}
void init_bomb(Bomb *bomb, Player *owner)
{
    bomb->is_active = true;
    bomb->owner = owner;
    bomb->sprite.src_rect = tileRects[BOMB];
    bomb->sprite.dst_rect = owner->sprite.dst_rect;
    bomb->timer = BOMB_LIFE_TIME;
    bomb->spread_timer = BOMB_SPREAD_LIFE_TIME;
    bomb->range = owner->bomb_range;
    bomb->spreadCoordsCount = 0;
    bomb->spreadCoords = NULL;
}

void add_bomb(Game *game, Player *owner)
{
}

bool place_bomb(Game *game, Player *owner)
{
    // place the bomb infron of the player depending on the direction
    if (owner->bomb_count <= 0)
        return false;

    SDL_Rect bomb_rect = owner->sprite.dst_rect;
    switch (owner->direction)
    {
    case LEFT:
        bomb_rect.x -= TILE_SIZE * SCALE;
        break;
    case RIGHT:
        bomb_rect.x += TILE_SIZE * SCALE;
        break;
    case UP:
        bomb_rect.y -= TILE_SIZE * SCALE;
        break;
    case DOWN:
        bomb_rect.y += TILE_SIZE * SCALE;
        break;
    default:
        break;
    }

    // check if the bomb can be placed
    if (can_place_bomb(game, &bomb_rect))
    {
        Bomb *bomb = &game->bombs[game->bomb_count++];
        init_bomb(bomb, owner);
        bomb->sprite.dst_rect = bomb_rect;
        game->map.data[bomb_rect.y / (TILE_SIZE * SCALE)][bomb_rect.x / (TILE_SIZE * SCALE)] = 'b';
        owner->bomb_count--;
        return true;
    }
    return false;
}

bool can_place_bomb(Game *game, SDL_Rect *rect)
{
    int row = rect->y / (TILE_SIZE * SCALE);
    int col = rect->x / (TILE_SIZE * SCALE);

    if (row < 0 || row >= game->map.height || col < 0 || col >= game->map.width)
    {
        return false;
    }

    return game->map.data[row][col] == ' ';
}

void update_bombs(Game *game)
{
    for (int i = 0; i < game->bomb_count; i++)
    {
        Bomb *bomb = &game->bombs[i];
        update_bomb(game, bomb);
    }
}

void update_bomb(Game *game, Bomb *bomb)
{

    if (bomb->is_active)
    {
        if (bomb->timer <= 0)
        {
            explode_bomb(game, bomb);
        }
        else
        {
            bomb->timer -= game->delta_time;
        }
    }
    else if (bomb->spreadCoordsCount > 0)
    {

        bomb->spread_timer -= game->delta_time;

        if (bomb->spread_timer <= 0)
        {

            for (int j = 0; j < bomb->spreadCoordsCount; j++)
            {
                game->map.data[bomb->spreadCoords[j].y][bomb->spreadCoords[j].x] = ' ';
            }
            bomb->spreadCoordsCount = 0;
            bomb->spread_timer = BOMB_SPREAD_LIFE_TIME;
        }
    }
}

void explode_bomb(Game *game, Bomb *bomb)
{
    if (!bomb->is_active)
        return;
    // remove the bomb from the map
    game->map.data[bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE)][bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE)] = 'x';

    // TODO: Free this memory allocation when the bomb is removed
    bomb->spreadCoords = malloc(sizeof(Coords) * bomb->range * 4);

    printf("Exploding bomb at (%d, %d)\n", bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE), bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE));

    // add explosion
    add_explosion(game, bomb, -1, bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE), bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE));

    // add explosion to the left
    for (int i = 1; i <= bomb->range; i++)
    {
        if (add_explosion(game, bomb, LEFT, bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE) - i, bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE)))
            break;
    }

    // add explosion to the right
    for (int i = 1; i <= bomb->range; i++)
    {
        if (add_explosion(game, bomb, RIGHT, bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE) + i, bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE)))
            break;
    }

    // add explosion to the top
    for (int i = 1; i <= bomb->range; i++)
    {
        if (add_explosion(game, bomb, UP, bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE), bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE) - i))
            break;
    }

    // add explosion to the bottom
    for (int i = 1; i <= bomb->range; i++)
    {
        if (add_explosion(game, bomb, DOWN, bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE), bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE) + i))
            break;
    }

    bomb->is_active = false;
    bomb->owner->bomb_count++;

    printf("Bomb exploded\nSpread coords count: %d\n", bomb->spreadCoordsCount);
}

// add explosion to the map
bool add_explosion(Game *game, Bomb *bomb, Direction direction, int col, int row)
{
    if (row < 0 || row >= game->map.height || col < 0 || col >= game->map.width)
    {
        if (row <= 0)
        {
            if (direction == UP)
            {
                game->map.data[1][col] = '^';
            }
            else if (direction == DOWN)
            {
                game->map.data[1][col] = 'v';
            }
            else if (direction == LEFT)
            {
                game->map.data[1][col] = '<';
            }
            else if (direction == RIGHT)
            {
                game->map.data[1][col] = '>';
            }
            else
            {
                game->map.data[1][col] = 'x';
            }

            bomb->spreadCoords[bomb->spreadCoordsCount].x = col;
            bomb->spreadCoords[bomb->spreadCoordsCount].y = 1;
            bomb->spreadCoordsCount++;

            return true;
        }
        else if (row >= game->map.height)
        {
            if (direction == UP)
            {
                game->map.data[game->map.height - 1][col] = '^';
            }
            else if (direction == DOWN)
            {
                game->map.data[game->map.height - 1][col] = 'v';
            }
            else if (direction == LEFT)
            {
                game->map.data[game->map.height - 1][col] = '<';
            }
            else if (direction == RIGHT)
            {
                game->map.data[game->map.height - 1][col] = '>';
            }
            else
            {
                game->map.data[game->map.height - 1][col] = 'x';
            }
            bomb->spreadCoords[bomb->spreadCoordsCount].x = col;
            bomb->spreadCoords[bomb->spreadCoordsCount].y = game->map.height - 1;
            bomb->spreadCoordsCount++;
            return true;
        }
        else if (col < 0)
        {
            if (direction == UP)
            {
                game->map.data[row][0] = '^';
            }
            else if (direction == DOWN)
            {
                game->map.data[row][0] = 'v';
            }
            else if (direction == LEFT)
            {
                game->map.data[row][0] = '<';
            }
            else if (direction == RIGHT)
            {
                game->map.data[row][0] = '>';
            }
            else
            {
                game->map.data[row][0] = 'x';
            }

            bomb->spreadCoords[bomb->spreadCoordsCount].x = 0;
            bomb->spreadCoords[bomb->spreadCoordsCount].y = row;
            bomb->spreadCoordsCount++;
            return true;
        }
        else if (col >= game->map.width)
        {
            if (direction == UP)
            {
                game->map.data[row][game->map.width - 1] = '^';
            }
            else if (direction == DOWN)
            {
                game->map.data[row][game->map.width - 1] = 'v';
            }
            else if (direction == LEFT)
            {
                game->map.data[row][game->map.width - 1] = '<';
            }
            else if (direction == RIGHT)
            {
                game->map.data[row][game->map.width - 1] = '>';
            }
            else
            {
                game->map.data[row][game->map.width - 1] = 'x';
            }

            bomb->spreadCoords[bomb->spreadCoordsCount].x = game->map.width - 1;
            bomb->spreadCoords[bomb->spreadCoordsCount].y = row;
            bomb->spreadCoordsCount++;

            return true;
        }
    }
    else if (game->map.data[row][col] == 'w')
    {
        return true;
    }

    if (game->map.data[row][col] == ' ')
    {
        if (direction == -1)
            game->map.data[row][col] = 'x';
        else if (direction == UP || direction == DOWN)
            game->map.data[row][col] = '|';
        else
            game->map.data[row][col] = '-';

        bomb->spreadCoords[bomb->spreadCoordsCount].x = col;
        bomb->spreadCoords[bomb->spreadCoordsCount].y = row;
        bomb->spreadCoordsCount++;

        return false;
    }
    else if (game->map.data[row][col] == 'b')
    {
        for (int i = 0; i < game->bomb_count; i++)
        {
            Bomb *bomb = &game->bombs[i];
            if (bomb->is_active && bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE) == col && bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE) == row)
            {
                explode_bomb(game, bomb);
                return true;
            }
        }
    }
    else if (game->map.data[row][col] == '.')
    {
        if (direction == UP)
            game->map.data[row][col] = '^';
        else if (direction == DOWN)
            game->map.data[row][col] = 'v';
        else if (direction == LEFT)
            game->map.data[row][col] = '<';
        else if (direction == RIGHT)
            game->map.data[row][col] = '>';
        else
            game->map.data[row][col] = 'x';

        bomb->spreadCoords[bomb->spreadCoordsCount].x = col;
        bomb->spreadCoords[bomb->spreadCoordsCount].y = row;
        bomb->spreadCoordsCount++;

        return true;
    }
    else if (game->map.data[row][col] == '1')
    {
        if (direction == UP)
            game->map.data[row][col] = '^';
        else if (direction == DOWN)
            game->map.data[row][col] = 'v';
        else if (direction == LEFT)
            game->map.data[row][col] = '<';
        else if (direction == RIGHT)
            game->map.data[row][col] = '>';
        else
            game->map.data[row][col] = 'x';

        bomb->spreadCoords[bomb->spreadCoordsCount].x = col;
        bomb->spreadCoords[bomb->spreadCoordsCount].y = row;
        bomb->spreadCoordsCount++;

        return true;
    }

    if (direction == UP)
        game->map.data[row][col] = '^';
    else if (direction == DOWN)
        game->map.data[row][col] = 'v';
    else if (direction == LEFT)
        game->map.data[row][col] = '<';
    else if (direction == RIGHT)
        game->map.data[row][col] = '>';
    else
        game->map.data[row][col] = 'x';

    bomb->spreadCoords[bomb->spreadCoordsCount].x = col;
    bomb->spreadCoords[bomb->spreadCoordsCount].y = row;
    bomb->spreadCoordsCount++;

    return true;
}

void free_bomb(Bomb *bomb)
{
    free(bomb->spreadCoords);
}