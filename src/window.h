#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "server.h"

typedef struct window
{
    SDL_Window *window;
    SDL_Surface *screenSurface;
    SDL_Renderer *renderer;
    int client_fd;
    char playerChar;
} Window;

bool init_window();

#endif // !_WINDOW_H_