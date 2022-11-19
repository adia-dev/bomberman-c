#include "window.h"

bool init_window()
{
    // the window we'll be rendering to
    SDL_Window *window = NULL;

    // the surface contained by the window
    SDL_Surface *screenSurface = NULL;

    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // create window
    const int WINDOW_WIDTH = MAP_WIDTH * CELL_SIZE;
    const int WINDOW_HEIGHT = MAP_HEIGHT * CELL_SIZE;
    window = SDL_CreateWindow("Bomberman", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    // get window surface
    screenSurface = SDL_GetWindowSurface(window);

    // connect the renderer to the window
    SDL_RenderPresent(renderer);

    int client_fd = init_client();
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, 0, MAX_BUFFER_SIZE);
    char playerChar = '\0';

    read_message(client_fd, buffer);
    playerChar = buffer[0];
    memset(buffer, 0, MAX_BUFFER_SIZE);

    printf("Player char: %c\n", playerChar);

    if (screenSurface == NULL)
    {
        printf("Window surface could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // main loop flag
    bool quit = false;

    // event handler
    SDL_Event e;

    // start time
    clock_t start = clock();

    // frame counter
    int frame = 0;

    char **map = NULL;

    float delta_time = 0.0f;
    float read_message_delay = 0.1f;
    float read_message_timer = 0.0f;

    // while application is running
    while (!quit)
    {
        // handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {
            // user requests quit
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            // handle keyboard
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_UP:
                case SDLK_w:
                    if (send_message(client_fd, "w") == -1)
                    {
                        printf("Error sending message\n");
                    }
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                    if (send_message(client_fd, "s") == -1)
                    {
                        printf("Error sending message\n");
                    }
                    break;
                case SDLK_LEFT:
                case SDLK_a:
                    if (send_message(client_fd, "a") == -1)
                    {
                        printf("Error sending message\n");
                    }
                    break;
                case SDLK_RIGHT:
                case SDLK_d:
                    if (send_message(client_fd, "d") == -1)
                    {
                        printf("Error sending message\n");
                    }
                    break;
                case SDLK_SPACE:
                    if (send_message(client_fd, " ") == -1)
                    {
                        printf("Error sending message\n");
                    }
                    break;
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                default:
                    printf("Key pressed: %c\n", e.key.keysym.sym);
                    break;
                }
            }
        }

        read_message_timer += 0.01f;

        if (read_message_timer >= read_message_delay)
        {
            if (non_blocking_read(client_fd, buffer) > 0)
            {
                printf("%s\n", buffer);
                if (map != NULL)
                {
                    free(map);
                }
                map = string_to_map(buffer);
                memset(buffer, 0, MAX_BUFFER_SIZE);
            }

            read_message_timer = 0.0f;
        }

        // fill the surface black
        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));

        // draw map
        if (map != NULL)
        {
            for (int i = 0; i < MAP_HEIGHT; i++)
            {
                for (int j = 0; j < MAP_WIDTH; j++)
                {
                    SDL_Rect rect = {j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE};

                    switch (map[i][j])
                    {
                    case 'x':
                        SDL_FillRect(screenSurface, &rect, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0xFF));
                        break;
                    case '1':
                        SDL_FillRect(screenSurface, &rect, SDL_MapRGB(screenSurface->format, 0xFF, 0x00, 0x00));
                        break;
                    case '2':
                        SDL_FillRect(screenSurface, &rect, SDL_MapRGB(screenSurface->format, 0x00, 0xFF, 0x00));
                        break;
                    default:
                        SDL_FillRect(screenSurface, &rect, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
                        break;
                    }
                }
            }
        }

        // update the surface
        SDL_UpdateWindowSurface(window);
    }

    // destroy window
    SDL_DestroyWindow(window);

    // clean up renderer
    SDL_DestroyRenderer(renderer);

    // quit SDL subsystems
    SDL_Quit();

    if (map != NULL)
    {
        free(map);
    }

    return true;
}
