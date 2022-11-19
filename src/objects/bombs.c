#include "bombs.h"

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
    bomb->spread_timer = BOMB_SPREAD_LIFE_TIME;
    bomb->timer = BOMB_LIFE_TIME;
    bomb->spreadCoordsCount = 0;
    bomb->spreadCoords = NULL;
    if (owner != NULL)
    {
        bomb->coords.x = owner->coords.x;
        bomb->coords.y = owner->coords.y;
        bomb->range = owner->bomb_range;
        bomb->sprite.dst_rect = owner->sprite.dst_rect;
    }
    else
    {
        bomb->coords.x = 0;
        bomb->coords.y = 0;
        bomb->range = 1;
        bomb->sprite.dst_rect = (SDL_Rect){0, 0, TILE_SIZE * SCALE, TILE_SIZE * SCALE};
    }
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
    for (int i = 0; i < MAX_NUMBER_OF_BOMBS; i++)
    {
        Bomb *bomb = &game->bombs[i];
        update_bomb(game, bomb);
    }
}

void update_bomb(Game *game, Bomb *bomb)
{

    if (bomb->owner == NULL)
        return;

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
            game->map.data[bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE)][bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE)] = ' ';
            bomb->spreadCoordsCount = 0;
            bomb->spread_timer = BOMB_SPREAD_LIFE_TIME;
        }
    }
}

void explode_bomb(Game *game, Bomb *bomb)
{
    if (!bomb->is_active)
        return;

    game->map.data[bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE)][bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE)] = 'x';

    // TODO: Free this memory allocation when the bomb is removed
    bomb->spreadCoords = malloc(sizeof(Coords) * bomb->range * 4);

    // add explosion
    add_explosion(game, bomb, -1, bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE), bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE), bomb->range);

    // add explosion to the left
    for (int i = 1; i <= bomb->range; i++)
    {
        if (add_explosion(game, bomb, LEFT, bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE) - i, bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE), i))
            break;
    }

    // add explosion to the right
    for (int i = 1; i <= bomb->range; i++)
    {
        if (add_explosion(game, bomb, RIGHT, bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE) + i, bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE), i))
            break;
    }

    // add explosion to the top
    for (int i = 1; i <= bomb->range; i++)
    {
        if (add_explosion(game, bomb, UP, bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE), bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE) - i, i))
            break;
    }

    // add explosion to the bottom
    for (int i = 1; i <= bomb->range; i++)
    {
        if (add_explosion(game, bomb, DOWN, bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE), bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE) + i, i))
            break;
    }

    bomb->is_active = false;
    bomb->owner->bomb_count++;
    game->bomb_count--;

    printf("Bomb exploded\nSpread coords count: %d\n", bomb->spreadCoordsCount);
}

// add explosion to the map
bool add_explosion(Game *game, Bomb *bomb, Direction direction, int col, int row, int range)
{
    if (row < 0 || row >= game->map.height || col < 0 || col >= game->map.width)
    {
        return true;
    }

    if (drop_powerup(game, col, row))
        return true;

    if (game->map.data[row][col] == 'x')
    {
        return true;
    }

    if (game->map.data[row][col] == 'b')
    {
        for (int i = 0; i < MAX_NUMBER_OF_BOMBS; i++)
        {
            Bomb *other_bomb = &game->bombs[i];
            if (other_bomb->sprite.dst_rect.x / (TILE_SIZE * SCALE) == col && other_bomb->sprite.dst_rect.y / (TILE_SIZE * SCALE) == row)
            {
                explode_bomb(game, other_bomb);
                break;
            }
        }
    }

    handle_player_explosion(game, bomb, col, row);

    if (game->map.data[row][col] == 'w')
    {
        handle_explosion_direction(game, bomb, direction, col, row, range);
        return true;
    }

    if (game->map.data[row][col] == '.')
    {

        handle_explosion_direction(game, bomb, direction, col, row, range);
        return true;
    }

    if (game->map.data[row][col] == ' ')
    {
        handle_explosion_direction(game, bomb, direction, col, row, range);
        return false;
    }

    return false;
}

void handle_explosion_direction(Game *game, Bomb *bomb, Direction direction, int col, int row, int range)
{

    if (col < 0 || col >= game->map.width || row <= 0 || row >= game->map.height || game->map.data[row][col] == 'x')
        return;

    switch (direction)
    {
    case UP:
        if (is_tile_wall(game->map.data[row - 1][col]))
        {
            game->map.data[row + 1][col] = '^';
            spread_explosion(game, bomb, col, row + 1);
        }
        else if (game->map.data[row][col] == '.' || range == bomb->range)
        {
            game->map.data[row][col] = '^';
            spread_explosion(game, bomb, col, row);
        }
        else if (game->map.data[row][col] == ' ')
        {
            game->map.data[row][col] = '|';
            spread_explosion(game, bomb, col, row);
        }
        break;
    case DOWN:
        if (game->map.data[row][col] == 'w')
        {
            game->map.data[row - 1][col] = 'v';
            spread_explosion(game, bomb, col, row - 1);
        }
        else if (game->map.data[row][col] == '.' || range == bomb->range)
        {
            game->map.data[row][col] = 'v';
            spread_explosion(game, bomb, col, row);
        }
        else if (game->map.data[row][col] == ' ')
        {
            game->map.data[row][col] = '|';
            spread_explosion(game, bomb, col, row);
        }
        break;
    case LEFT:
        if (game->map.data[row][col] == 'w')
        {
            game->map.data[row][col + 1] = '<';
            spread_explosion(game, bomb, col + 1, row);
        }
        else if (game->map.data[row][col] == '.' || range == bomb->range)
        {
            game->map.data[row][col] = '<';
            spread_explosion(game, bomb, col, row);
        }
        else if (game->map.data[row][col] == ' ')
        {
            game->map.data[row][col] = '-';
            spread_explosion(game, bomb, col, row);
        }
        break;
    case RIGHT:
        if (game->map.data[row][col] == 'w')
        {
            game->map.data[row][col - 1] = '>';
            spread_explosion(game, bomb, col - 1, row);
        }
        else if (game->map.data[row][col] == '.' || range == bomb->range)
        {
            game->map.data[row][col] = '>';
            spread_explosion(game, bomb, col, row);
        }
        else if (game->map.data[row][col] == ' ')
        {
            game->map.data[row][col] = '-';
            spread_explosion(game, bomb, col, row);
        }
        break;
    default:
        break;
    }
}

void handle_player_explosion(Game *game, Bomb *bomb, int col, int row)
{
    if (is_tile_player(game->map.data[row][col]))
    {
        for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++)
        {
            Player *player = &game->players[i];
            if (player->sprite.dst_rect.x / (TILE_SIZE * SCALE) == col && player->sprite.dst_rect.y / (TILE_SIZE * SCALE) == row)
            {
                take_damage(game, player, bomb->damage);
                bomb->owner->score++;
                break;
            }
        }
    }
}

bool drop_powerup(Game *game, int col, int row)
{
    if (game->map.data[row][col] == '.')
    {
        // drop powerup randomly between 0.0 and 1.0
        float random = (float)rand() / (float)RAND_MAX;
        if (random < POWERUP_DROP_RATE)
        {
            game->map.data[row][col] = '?';
            return true;
        }
    }

    return false;
}

void spread_explosion(Game *game, Bomb *bomb, int col, int row)
{
    bomb->spreadCoords[bomb->spreadCoordsCount].x = col;
    bomb->spreadCoords[bomb->spreadCoordsCount].y = row;
    bomb->spreadCoordsCount++;
}

void free_bomb(Bomb *bomb)
{
    free(bomb->spreadCoords);
}
