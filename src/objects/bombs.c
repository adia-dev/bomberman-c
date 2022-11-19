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