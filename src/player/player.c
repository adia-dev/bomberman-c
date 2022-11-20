#include "player.h"

void init_player(Game *game, Player *player, int x, int y)
{
    player->coords.x = x;
    player->coords.y = y;
    player->bomb_range = 2;
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

    // powerups
    player->can_kick = false;
    player->can_teleport = false;
    player->can_survive = false;
    player->invincible_timer = 0;
    player->powerup_color.r = 255;
    player->powerup_color.g = 255;
    player->powerup_color.b = 255;
    player->powerup_color.a = 255;

    SDL_Rect src_rect = {0, 0, TILE_SIZE, TILE_SIZE};
    SDL_Rect dst_rect = {x * TILE_SIZE * SCALE, y * TILE_SIZE * SCALE, TILE_SIZE * SCALE, TILE_SIZE * SCALE};

    player->sprite.src_rect = src_rect;
    player->sprite.dst_rect = dst_rect;

    player->id = game->player_count;

    game->map.data[y][x] = get_player_tile(player);
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
        if (!player->is_dead)
            draw_player(game, player);
    }
}

void draw_player(Game *game, Player *player)
{
    update_player_rect(player);

    // SDL_RenderCopy(game->renderer, game->texture, &player->sprite.src_rect, &player->sprite.dst_rect);
    if (player->invincible_timer > 1.f)
    {
        SDL_SetTextureColorMod(game->texture, 200, 155, 255);
        SDL_RenderCopy(game->renderer, game->texture, &player->sprite.src_rect, &player->sprite.dst_rect);
        SDL_SetTextureColorMod(game->texture, 255, 255, 255);
    }
    else if (player->is_powerup)
    {
        SDL_SetTextureColorMod(game->texture, player->powerup_color.r, player->powerup_color.g, player->powerup_color.b);
        SDL_RenderCopy(game->renderer, game->texture, &player->sprite.src_rect, &player->sprite.dst_rect);
        SDL_SetTextureColorMod(game->texture, 255, 255, 255);
    }
    else if (player->can_survive)
    {
        SDL_SetTextureColorMod(game->texture, 0, 0, 255);
        SDL_RenderCopy(game->renderer, game->texture, &player->sprite.src_rect, &player->sprite.dst_rect);
        SDL_SetTextureColorMod(game->texture, 255, 255, 255);
    }
    else
    {
        SDL_RenderCopy(game->renderer, game->texture, &player->sprite.src_rect, &player->sprite.dst_rect);
    }

    draw_player_powerups(game, player);
}

void draw_player_powerups(Game *game, Player *player)
{
    SDL_Rect powerup_rect = get_powerup_rect(-1);
    SDL_Rect powerup_dst_rect = {0, window_height - UI_SIZE, UI_SIZE, UI_SIZE};

    int x = 0;

    if (player->invincible_timer > 1.f)
    {
        powerup_rect = get_powerup_rect(POWERUP_INVINCIBLE);
        powerup_dst_rect.x = x;
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
        x += UI_SIZE;
    }

    if (player->can_survive)
    {
        powerup_rect = get_powerup_rect(POWERUP_SHIELD);
        powerup_dst_rect.x = x;
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
        x += UI_SIZE;
    }

    if (player->can_kick)
    {
        powerup_rect = get_powerup_rect(POWERUP_KICK);
        powerup_dst_rect.x = x;
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
        x += UI_SIZE;
    }

    if (player->can_teleport)
    {
        powerup_rect = get_powerup_rect(POWERUP_TELEPORT);
        powerup_dst_rect.x = x;
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
        x += UI_SIZE;
    }

    if (player->bomb_range == 1)
    {
        powerup_rect = get_powerup_rect(POWERUP_RANGE_DOWN);
        powerup_dst_rect.x = x;
        // red color
        SDL_SetTextureColorMod(game->texture, 255, 0, 0);
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
        // reset color
        SDL_SetTextureColorMod(game->texture, 255, 255, 255);
        x += UI_SIZE;
    }
    else if (player->bomb_range == MAX_BOMB_RANGE)
    {
        powerup_rect = get_powerup_rect(POWERUP_RANGE_UP);
        powerup_dst_rect.x = x;
        // green color
        SDL_SetTextureColorMod(game->texture, 0, 255, 0);
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
        // reset color
        SDL_SetTextureColorMod(game->texture, 255, 255, 255);
        x += UI_SIZE;
        printf("MAX BOMB RANGE\n");
    }

    if (player->bomb_count == 1)
    {
        powerup_rect = get_powerup_rect(POWERUP_BOMB_DOWN);
        powerup_dst_rect.x = x;
        // red color
        SDL_SetTextureColorMod(game->texture, 255, 0, 0);
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
        // reset color
        SDL_SetTextureColorMod(game->texture, 255, 255, 255);
        x += UI_SIZE;
    }
    else if (player->bomb_count == MAX_NUMBER_OF_BOMBS)
    {
        powerup_rect = get_powerup_rect(POWERUP_BOMB_UP);
        powerup_dst_rect.x = x;
        // green color
        SDL_SetTextureColorMod(game->texture, 0, 255, 0);
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
        // reset color
        SDL_SetTextureColorMod(game->texture, 255, 255, 255);
        x += UI_SIZE;
    }

    if (x != 0)
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
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

        int x = player_rect.x / (TILE_SIZE * SCALE);
        int y = player_rect.y / (TILE_SIZE * SCALE);

        Powerup *powerup_in_the_way = collide_with_powerup(game, x, y);
        if (powerup_in_the_way != NULL)
        {
            consume_powerup(game, player, powerup_in_the_way);
        }

        player->direction = direction;
        player->sprite.dst_rect = player_rect;

        game->map.data[old_y][old_x] = ' ';
        game->map.data[y][x] = get_player_tile(player);
    }
}

Powerup *collide_with_powerup(Game *game, int x, int y)
{
    for (int i = 0; i < MAX_NUMBER_OF_POWERUPS; i++)
    {
        Powerup *powerup = &game->powerups[i];
        // if (other_bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE) == col && other_bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE) == row)
        if (powerup->is_active && powerup->sprite.dst_rect.x / (TILE_SIZE * SCALE) == x && powerup->sprite.dst_rect.y / (TILE_SIZE * SCALE) == y)
        {
            return powerup;
        }
    }

    return NULL;
}

bool consume_powerup(Game *game, Player *player, Powerup *powerup)
{

    player->is_powerup = true;
    player->powerup = powerup->type;
    player->powerup_timer = powerup->timer;

    // update the powerup flags for the player based on the powerup type
    switch (powerup->type)
    {
    case POWERUP_BOMB_UP:
        player->bomb_count++;
        break;
    case POWERUP_BOMB_DOWN:
        player->bomb_count = max(player->bomb_count--, 1);
        break;
    case POWERUP_RANGE_UP:
        player->bomb_range++;
        break;
    case POWERUP_RANGE_DOWN:
        player->bomb_range = (player->bomb_range--, 1);
        break;
    case POWERUP_RANGE_MAX:
        player->bomb_range = MAX_BOMB_RANGE;
        break;
    case POWERUP_TELEPORT:
        player->can_teleport = true;
        break;
    case POWERUP_KICK:
        player->can_kick = true;
        break;
    case POWERUP_SHIELD:
        player->can_survive = true;
        break;
    case POWERUP_INVINCIBLE:
        printf("Player %d is invincible\n", player->id);
        player->invincible_timer = POWERUP_INVINCIBLE_DURATION;
        break;
    default:
        break;
    }

    powerup->is_active = false;
    printf("Collided with powerup %s\n", get_powerup_name(powerup->type));

    game->map.data[powerup->sprite.dst_rect.y / (TILE_SIZE * SCALE)][powerup->sprite.dst_rect.x / (TILE_SIZE * SCALE)] = ' ';
    game->powerup_count--;

    return true;
}

bool is_valid_move(Game *game, SDL_Rect *rect)
{
    int row = rect->y / (TILE_SIZE * SCALE);
    int col = rect->x / (TILE_SIZE * SCALE);

    if (row < 0 || row >= game->map.height || col < 0 || col >= game->map.width)
    {
        return false;
    }

    return is_tile_empty(game->map.data[row][col]) || is_tile_powerup(game->map.data[row][col]) || is_tile_explosion(game->map.data[row][col]);
}

void update_players(Game *game)
{
    for (int i = 0; i < game->player_count; i++)
    {
        Player *player = &game->players[i];
        if (!player->is_dead)
            update_player(game, player);
    }
}

void update_player(Game *game, Player *player)
{
    player->invincible_timer -= game->delta_time / 1000.0;
    if (player->invincible_timer <= 0)
    {
        player->invincible_timer = 0.0f;
    }
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

void take_damage(Game *game, Player *player, int damage)
{
    if (player->is_powerup)
    {
        player->is_powerup = false;
        player->can_survive = false;
        player->powerup = 0;
    }
    else
    {
        player->lives -= damage;
        if (player->lives <= 0)
        {
            kill_player(game, player);
        }
    }
}

bool kill_player(Game *game, Player *player)
{
    if (player->is_dead)
        return false;

    printf("Player %d died\n", player->id);

    player->is_dead = true;
    player->lives--;
    player->score -= 100;

    if (player->lives <= 0)
    {
        game->player_count--;
        return true;
    }

    return false;
}

const char *get_powerup_name(PowerupType type)
{
    switch (type)
    {
    case POWERUP_BOMB_UP:
        return "Bomb Up";
        break;
    case POWERUP_BOMB_DOWN:
        return "Bomb Down";
        break;
    case POWERUP_RANGE_UP:
        return "Range Up";
        break;
    case POWERUP_RANGE_DOWN:
        return "Range Down";
        break;
    case POWERUP_RANGE_MAX:
        return "Range Max";
        break;
    case POWERUP_TELEPORT:
        return "Teleport";
        break;
    case POWERUP_KICK:
        return "Kick";
        break;
    case POWERUP_SHIELD:
        return "Shield";
        break;
    case POWERUP_INVINCIBLE:
        return "Invincible";
        break;
    case POWERUP_LIFE:
        return "Life";
        break;
    default:
        return "Unknown";
        break;
    }
    return "Unknown";
}
SDL_Rect get_powerup_rect(PowerupType type)
{
    switch (type)
    {
    case POWERUP_BOMB_UP:
    case POWERUP_BOMB_DOWN:
        return (SDL_Rect){0 * TILE_SIZE, 14 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        break;
    case POWERUP_RANGE_UP:
    case POWERUP_RANGE_DOWN:
    case POWERUP_RANGE_MAX:
        return (SDL_Rect){1 * TILE_SIZE, 14 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        break;
    case POWERUP_TELEPORT:
        return (SDL_Rect){3 * TILE_SIZE, 14 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        break;
    case POWERUP_KICK:
        return (SDL_Rect){5 * TILE_SIZE, 14 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        break;
    case POWERUP_INVINCIBLE:
        return (SDL_Rect){6 * TILE_SIZE, 14 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        break;
    case POWERUP_SHIELD:
        return (SDL_Rect){4 * TILE_SIZE, 14 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        break;
    case POWERUP_LIFE:
        return (SDL_Rect){2 * TILE_SIZE, 14 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        break;
    default:
        return (SDL_Rect){0 * TILE_SIZE, 14 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        break;
    }

    return (SDL_Rect){0 * TILE_SIZE, 14 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
}
