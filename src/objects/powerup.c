#include "powerup.h"

void init_powerups(Game *game)
{
    for (int i = 0; i < MAX_NUMBER_OF_POWERUPS; i++)
    {
        Powerup *powerup = &game->powerups[i];
        init_powerup(powerup);
    }
}

void init_powerup(Powerup *powerup)
{
    powerup->is_active = false;
    powerup->type = POWERUP_BOMB_UP;
    powerup->sprite.src_rect = tileRects[POWERUP];
    powerup->sprite.dst_rect = (SDL_Rect){0, 0, TILE_SIZE * SCALE, TILE_SIZE * SCALE};
    powerup->lifetime = POWERUP_LIFE_TIME;
    powerup->timer = 0;
}

void update_powerups(Game *game)
{
    for (int i = 0; i < MAX_NUMBER_OF_POWERUPS; i++)
    {
        Powerup *powerup = &game->powerups[i];
        update_powerup(game, powerup);
    }
}

void update_powerup(Game *game, Powerup *powerup)
{
    if (powerup->is_active)
    {
        powerup->timer += game->delta_time;
        if (powerup->timer >= powerup->lifetime)
        {
            init_powerup(powerup);
        }
    }
}

void draw_powerup(Game *game, Powerup *powerup)
{
    if (powerup->is_active)
    {
        SDL_RenderCopy(game->renderer, game->texture, &powerup->sprite.src_rect, &powerup->sprite.dst_rect);
    }
}

void draw_powerups(Game *game)
{
    for (int i = 0; i < MAX_NUMBER_OF_POWERUPS; i++)
    {
        Powerup *powerup = &game->powerups[i];
        draw_powerup(game, powerup);
    }
}

bool spawn_powerup(Game *game, int x, int y)
{
    if (x < 0 || x >= game->map.width || y < 0 || y >= game->map.height || !is_tile_empty(game->map.data[y][x]))
        return false;

    PowerupType random_powerup = rand() % POWERUP_COUNT;

    Powerup *powerup = &game->powerups[game->powerup_count++];
    powerup->is_active = true;
    powerup->type = random_powerup;

    SDL_Rect powerup_rect = tileRects[POWERUP];
    powerup_rect.x += random_powerup * powerup_rect.w;

    powerup->sprite.src_rect = powerup_rect;
    powerup->sprite.dst_rect.x = x * TILE_SIZE * SCALE;
    powerup->sprite.dst_rect.y = y * TILE_SIZE * SCALE;

    game->map.data[y][x] = '?';
    return true;
}

void spawn_powerup_randomly(Game *game)
{
    // spawn a powerup at a random empty tile on the map
    int x = rand() % game->map.width;
    int y = rand() % game->map.height;

    while (!spawn_powerup(game, x, y))
    {
        x = rand() % game->map.width;
        y = rand() % game->map.height;
    }
}
