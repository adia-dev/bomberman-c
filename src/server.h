#ifndef _server_H_
#define _server_H_

#include "sockets.h"

typedef struct Client
{
    int socketfd;
    char *name;
    struct sockaddr_in address;
} Client;

typedef struct Server
{
    int socketfd;
    struct sockaddr_in address;
    int client_count;
    fd_set readfds, writefds;
    char buffer[MAX_BUFFER_SIZE];
    Client *clients[MAX_CLIENT_COUNT];
} Server;

typedef struct Coords
{
    int x;
    int y;
} Coords;

char *map_to_string(char map[MAP_HEIGHT][MAP_WIDTH]);
char **string_to_map(const char *string);
int init_server();

Coords get_player_coords(char map[MAP_HEIGHT][MAP_WIDTH], char player);
Coords maybe_move_cell(Coords coords, char map[MAP_HEIGHT][MAP_WIDTH], char player, char direction);

#endif // !_server_H_
