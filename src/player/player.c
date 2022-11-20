#include "player.h"

void init_player(Game *game, Player *player, int x, int y)
{
    player->coords.x = x;
    player->coords.y = y;
    player->bomb_range = 2;
    player->bomb_count = 100;
    player->bomb_timer = 0;
    player->speed = 1;
    player->lives = 3;
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
    player->already_survived = false;
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

    player->spawn = playerInitialCoords[game->player_count - 1];

    game->map.data[y][x] = get_player_tile(player);
}

void add_player(Game *game)
{

    if (game->player_count < 4)
    {
        Player *player = &game->players[game->player_count++];
        int x = playerInitialCoords[game->player_count - 1].x;
        int y = playerInitialCoords[game->player_count - 1].y;
        player->id = game->player_count;

        printf("\t- Player[%d]: %p\n", player->id, player);

        if (x == -1)
            x = game->map.width - 2;
        if (y == -1)
            y = game->map.height - 2;

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
    else if (player->can_survive)
    {
        SDL_SetTextureColorMod(game->texture, 100, 100, 100);
        SDL_RenderCopy(game->renderer, game->texture, &player->sprite.src_rect, &player->sprite.dst_rect);
        SDL_SetTextureColorMod(game->texture, 255, 255, 255);
    }
    else
    {
        SDL_RenderCopy(game->renderer, game->texture, &player->sprite.src_rect, &player->sprite.dst_rect);
    }

    draw_player_powerups(game, player);
    draw_player_lives(game, player);
}

void draw_player_powerups(Game *game, Player *player)
{
    SDL_Rect powerup_rect = get_powerup_rect(-1);
    SDL_Rect powerup_dst_rect = {0, window_height - UI_SIZE, UI_SIZE, UI_SIZE};

    int x = 0;

    if (player->invincible_timer > 1.f)
    {
        powerup_rect = get_powerup_rect(POWERUP_INVINCIBLE);
        SDL_Color color = get_powerup_color(POWERUP_INVINCIBLE);
        powerup_dst_rect.x = x;
        SDL_SetTextureColorMod(game->texture, color.r, color.g, color.b);
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
        SDL_SetTextureColorMod(game->texture, 255, 255, 255);
        x += UI_SIZE;
    }

    if (player->can_survive)
    {
        powerup_rect = get_powerup_rect(POWERUP_SHIELD);
        SDL_Color color = get_powerup_color(POWERUP_SHIELD);
        powerup_dst_rect.x = x;
        SDL_SetTextureColorMod(game->texture, color.r, color.g, color.b);
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
        SDL_SetTextureColorMod(game->texture, 255, 255, 255);
        x += UI_SIZE;
    }

    if (player->can_kick)
    {
        powerup_rect = get_powerup_rect(POWERUP_KICK);
        SDL_Color color = get_powerup_color(POWERUP_KICK);
        powerup_dst_rect.x = x;
        SDL_SetTextureColorMod(game->texture, color.r, color.g, color.b);
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
        SDL_SetTextureColorMod(game->texture, 255, 255, 255);
        x += UI_SIZE;
    }

    if (player->can_teleport)
    {
        powerup_rect = get_powerup_rect(POWERUP_TELEPORT);
        SDL_Color color = get_powerup_color(POWERUP_TELEPORT);
        powerup_dst_rect.x = x;
        SDL_SetTextureColorMod(game->texture, color.r, color.g, color.b);
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
        SDL_SetTextureColorMod(game->texture, color.r, color.g, color.b);
        x += UI_SIZE;
    }

    if (player->bomb_range == 1)
    {
        powerup_rect = get_powerup_rect(POWERUP_RANGE_DOWN);
        SDL_Color color = get_powerup_color(POWERUP_RANGE_DOWN);
        powerup_dst_rect.x = x;
        SDL_SetTextureColorMod(game->texture, color.r, color.g, color.b);
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
        SDL_SetTextureColorMod(game->texture, 255, 255, 255);
        x += UI_SIZE;
    }
    else if (player->bomb_range == MAX_BOMB_RANGE)
    {
        powerup_rect = get_powerup_rect(POWERUP_RANGE_UP);
        SDL_Color color = get_powerup_color(POWERUP_RANGE_UP);
        powerup_dst_rect.x = x;
        SDL_SetTextureColorMod(game->texture, color.r, color.g, color.b);
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
        SDL_SetTextureColorMod(game->texture, 255, 255, 255);
        x += UI_SIZE;
    }

    if (player->bomb_count == 1)
    {
        powerup_rect = get_powerup_rect(POWERUP_BOMB_DOWN);
        SDL_Color color = get_powerup_color(POWERUP_BOMB_DOWN);
        powerup_dst_rect.x = x;
        SDL_SetTextureColorMod(game->texture, color.r, color.g, color.b);
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
        SDL_SetTextureColorMod(game->texture, 255, 255, 255);
        x += UI_SIZE;
    }
    else if (player->bomb_count == MAX_NUMBER_OF_BOMBS)
    {
        powerup_rect = get_powerup_rect(POWERUP_BOMB_UP);
        SDL_Color color = get_powerup_color(POWERUP_BOMB_UP);
        powerup_dst_rect.x = x;
        SDL_SetTextureColorMod(game->texture, color.r, color.g, color.b);
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
        SDL_SetTextureColorMod(game->texture, 255, 255, 255);
        x += UI_SIZE;
    }

    if (x != 0)
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
}

void draw_player_lives(Game *game, Player *player)
{

    SDL_Rect powerup_rect = get_powerup_rect(POWERUP_LIFE);
    SDL_Color color = get_powerup_color(POWERUP_LIFE);
    SDL_Rect powerup_dst_rect = {0, window_height - UI_SIZE, UI_SIZE, UI_SIZE};

    // printf("player lives: %d\n", player->lives);

    SDL_SetTextureColorMod(game->texture, color.r, color.g, color.b);
    int x = window_width - UI_SIZE;

    for (size_t i = 0; i < player->lives; i++)
    {
        powerup_dst_rect.x = x;
        SDL_RenderCopy(game->renderer, game->texture, &powerup_rect, &powerup_dst_rect);
        x -= UI_SIZE;
    }
    SDL_SetTextureColorMod(game->texture, 255, 255, 255);
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

    if (is_valid_move(game, player, &player_rect))
    {

        int x = player_rect.x / (TILE_SIZE * SCALE);
        int y = player_rect.y / (TILE_SIZE * SCALE);

        if (teleport_player(game, player, x, y))
            return;

        Powerup *powerup_in_the_way = collide_with_powerup(game, x, y);
        if (powerup_in_the_way != NULL)
        {
            consume_powerup(game, player, powerup_in_the_way);
        }

        Bomb *bomb_in_the_way = collide_with_bomb(game, x, y);
        if (bomb_in_the_way != NULL)
        {
            printf("BOMB IN THE WAY\nKicking it: %d\n", player->direction);
            kick_bomb(game, bomb_in_the_way, direction);
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

Bomb *collide_with_bomb(Game *game, int x, int y)
{
    for (int i = 0; i < MAX_NUMBER_OF_BOMBS; i++)
    {
        Bomb *other_bomb = &game->bombs[i];
        // if (other_bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE) == col && other_bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE) == row)
        if (other_bomb->is_active && other_bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE) == x && other_bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE) == y)
        {
            return other_bomb;
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
        player->can_kick = false;
        break;
    case POWERUP_KICK:
        player->can_kick = true;
        player->can_teleport = false;
        break;
    case POWERUP_SHIELD:
        if (!player->already_survived)
            player->can_survive = true;
        break;
    case POWERUP_INVINCIBLE:
        printf("Player %d is invincible\n", player->id);
        player->invincible_timer = POWERUP_INVINCIBLE_DURATION;
        break;
    case POWERUP_LIFE:
        player->lives++;
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

bool is_valid_move(Game *game, Player *player, SDL_Rect *rect)
{
    int row = rect->y / (TILE_SIZE * SCALE);
    int col = rect->x / (TILE_SIZE * SCALE);

    if (row < 0 || row >= game->map.height || col < 0 || col >= game->map.width)
    {
        return false;
    }

    return is_tile_empty(game->map.data[row][col]) || is_tile_powerup(game->map.data[row][col]) || (player->can_kick && is_tile_bomb(game->map.data[row][col]) || (player->can_teleport && is_tile_bomb(game->map.data[row][col])));
}

bool kick_bomb(Game *game, Bomb *bomb, Direction direction)
{
    if (game == NULL || bomb == NULL)
    {
        return false;
    }

    bomb->is_moving = true;
    bomb->direction = direction;

    return true;
}

bool teleport_player(Game *game, Player *player, int x, int y)
{
    // try to teleport the player to behind the bomb or group of bombs
    // if there is a bomb in the way, teleport the player to the other side of the bomb

    if (game == NULL || player == NULL || !player->can_teleport)
    {
        return false;
    }

    SDL_Rect player_rect = player->sprite.dst_rect;

    int old_x = player_rect.x / (TILE_SIZE * SCALE);
    int old_y = player_rect.y / (TILE_SIZE * SCALE);

    Bomb *bomb_in_the_way = NULL;

    while ((bomb_in_the_way = collide_with_bomb(game, x, y)) != NULL)
    {
        switch (player->direction)
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

        x = player_rect.x / (TILE_SIZE * SCALE);
        y = player_rect.y / (TILE_SIZE * SCALE);
    }

    if (is_valid_move(game, player, &player_rect))
    {
        player->sprite.dst_rect = player_rect;
        game->map.data[old_y][old_x] = ' ';
        game->map.data[y][x] = 'P';
        return true;
    }

    return false;
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

    if (player->invincible_timer > 0.0f)
    {
        return;
    }

    if (player->can_survive)
    {
        player->can_survive = false;
        player->already_survived = true;
        return;
    }

    player->lives -= damage;
    printf("Player %d has %d lives left\n", player->id, player->lives);
    if (player->lives <= 0)
    {
        kill_player(game, player);
    }
    else
    {

        // empty the player's position on the map
        int row = player->sprite.dst_rect.y / (TILE_SIZE * SCALE);
        int col = player->sprite.dst_rect.x / (TILE_SIZE * SCALE);
        game->map.data[row][col] = ' ';

        // move the player to its spawn point
        player->sprite.dst_rect.x = player->spawn.x * TILE_SIZE * SCALE;
        player->sprite.dst_rect.y = player->spawn.y * TILE_SIZE * SCALE;

        game->map.data[player->spawn.y][player->spawn.x] = get_player_tile(player);
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
    case POWERUP_LIFE:
        return (SDL_Rect){4 * TILE_SIZE, 14 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        break;
    default:
        return (SDL_Rect){0 * TILE_SIZE, 14 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        break;
    }

    return (SDL_Rect){0 * TILE_SIZE, 14 * TILE_SIZE, TILE_SIZE, TILE_SIZE};
}

SDL_Color get_powerup_color(PowerupType type)
{
    switch (type)
    {
    case POWERUP_BOMB_DOWN:
        return (SDL_Color){255, 0, 0, 255};
        break;
    case POWERUP_RANGE_DOWN:
        return (SDL_Color){255, 0, 0, 255};
        break;
    case POWERUP_RANGE_MAX:
        return (SDL_Color){0, 255, 0, 255};
        break;
    case POWERUP_LIFE:
        return (SDL_Color){0, 255, 0, 255};
        break;
    case POWERUP_SHIELD:
        return (SDL_Color){200, 100, 200, 255};
        break;
    default:
        return (SDL_Color){255, 255, 255, 255};
        break;
    }
}
