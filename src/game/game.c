#include "game.h"

void init_game(Game *game)
{
    game->is_running = true;
    game->window = NULL;
    game->renderer = NULL;
    game->texture = NULL;
    game->player_count = 0;
    game->bomb_count = 0;
    game->powerup_count = 0;
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
    init_bombs(game);
    init_powerups(game);

    load_map(&game->map, "../src/levels/level_01.txt");
    load_texture(game, "../src/assets/NES_BOMBERMAN.png");

    // resize window the size of a TILE_SIZE and the map width and height
    SDL_SetWindowSize(game->window, game->map.width * TILE_SIZE * SCALE, game->map.height * TILE_SIZE * SCALE);
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
    Player *current_player = &game->players[0];

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
            case SDLK_TAB:
                // switch between the 4 players
                game->main_player_id = (game->main_player_id + 1) % game->player_count;
                Player *main_player = &game->players[game->main_player_id];
                current_player = main_player;
                printf("Main player id: %d\n", game->main_player_id);
                break;
            case SDLK_SPACE:
                spawn_powerup_randomly(game);
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
    update_powerups(game);
}

void render(Game *game)
{
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);

    draw_map(game);
    draw_powerups(game);
    draw_bombs(game);
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
