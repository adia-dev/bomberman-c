#include "map.h"

void init_map(Map *map)
{
    map->width = 0;
    map->height = 0;
    map->data = NULL;
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

bool is_tile_player(char tile)
{
    return tile == '1' || tile == '2' || tile == '3' || tile == '4';
}

bool is_tile_wall(char tile)
{
    return tile == 'w';
}

bool is_tile_breakable(char tile)
{
    return tile == '.';
}

bool is_tile_bomb(char tile)
{
    return tile == 'b';
}

bool is_tile_explosion(char tile)
{
    return tile == '^' || tile == 'v' || tile == '<' || tile == '>' || tile == 'x' || tile == '-' || tile == '|';
}

bool is_tile_powerup(char tile)
{
    return tile == '?';
}

bool is_tile_empty(char tile)
{
    return tile == ' ';
}