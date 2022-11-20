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
    bomb->is_active = false;
    bomb->is_moving = false;
    bomb->owner = owner;
    bomb->sprite.src_rect = tileRects[BOMB];
    bomb->spread_timer = BOMB_SPREAD_LIFE_TIME;
    bomb->timer = BOMB_LIFE_TIME;
    bomb->move_delay = BOMB_MOVE_DELAY;
    bomb->move_timer = BOMB_MOVE_DELAY;
    bomb->spreadCoordsCount = 0;
    bomb->spreadCoords = NULL;
    bomb->animation_timer = 0.0;
    bomb->animation_frame_delay = BOMB_ANIMATION_FRAME_DELAY;
    bomb->animation_frame_count = 3;
    bomb->current_frame = 0;

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
    if (owner != NULL)
    {
        owner->bomb_count++;
    }
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
        bomb->is_active = true;
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
        if (bomb->is_moving)
        {
            bomb->move_timer -= game->delta_time;
            printf("move timer: %f\n", bomb->move_timer);

            if (bomb->move_timer <= 0.0)
            {
                bomb->move_delay = lerp(bomb->move_delay, 1000, 0.1);
                bomb->move_timer = bomb->move_delay;
                move_bomb(game, bomb, bomb->direction);
            }
        }

        if (bomb->timer <= 0)
        {
            explode_bomb(game, bomb);
        }
        else
        {
            bomb->animation_timer += game->delta_time;
            bomb->timer -= game->delta_time;
        }

        if (bomb->animation_timer >= bomb->animation_frame_delay)
        {
            bomb->animation_timer = 0.0;
            bomb->current_frame = (bomb->current_frame + 1) % bomb->animation_frame_count;
            bomb->sprite.src_rect.x = bomb->current_frame * TILE_SIZE;
            bomb->animation_frame_delay = lerp(bomb->animation_frame_delay, 25, 0.2);
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

void move_bomb(Game *game, Bomb *bomb, Direction direction)
{
    if (bomb->is_active)
    {
        SDL_Rect rect = bomb->sprite.dst_rect;
        int old_row = rect.y / (TILE_SIZE * SCALE);
        int old_col = rect.x / (TILE_SIZE * SCALE);
        switch (direction)
        {
        case LEFT:
            rect.x -= TILE_SIZE * SCALE;
            break;
        case RIGHT:
            rect.x += TILE_SIZE * SCALE;
            break;
        case UP:
            rect.y -= TILE_SIZE * SCALE;
            break;
        case DOWN:
            rect.y += TILE_SIZE * SCALE;
            break;
        default:
            break;
        }

        if (can_place_bomb(game, &rect))
        {
            bomb->sprite.dst_rect = rect;

            game->map.data[old_row][old_col] = ' ';
        }
        else
        {
            bomb->is_moving = false;
        }
    }
}

void draw_bombs(Game *game)
{
    for (int i = 0; i < MAX_NUMBER_OF_BOMBS; i++)
    {
        Bomb *bomb = &game->bombs[i];
        draw_bomb(game, bomb);
    }
}

void draw_bomb(Game *game, Bomb *bomb)
{
    if (bomb->is_active)
    {
        SDL_RenderCopy(game->renderer, game->texture, &bomb->sprite.src_rect, &bomb->sprite.dst_rect);
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

    if (col < 0 || col >= game->map.width || row <= 0 || row >= game->map.height)
        return;

    switch (direction)
    {
    case UP:
        if (is_tile_wall(game->map.data[row][col]))
        {
            if (range != 1)
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
            if (range != 1)
                game->map.data[row - 1][col] = 'v';
            spread_explosion(game, bomb, col, row - 1);
        }
        else if (game->map.data[row][col] == '.' || range == bomb->range)
        {
            if (range != 1)
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
            if (range != 1)
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
            if (range != 1)
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
                if (player->invincible_timer > 0)
                {
                    player->invincible_timer = 0;
                    break;
                }

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
            return spawn_powerup(game, col, row);
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
