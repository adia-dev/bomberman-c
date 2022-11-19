#include "player.h"

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
        draw_player(game, player);
    }
}

void draw_player(Game *game, Player *player)
{
    update_player_rect(player);

    SDL_RenderCopy(game->renderer, game->texture, &player->sprite.src_rect, &player->sprite.dst_rect);
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