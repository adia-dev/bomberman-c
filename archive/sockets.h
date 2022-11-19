#ifndef _SOCKETS_
#define _SOCKETS_

#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SERVER_PORT 4000
#define SERVER_BACKLOG_SIZE 10
#define SOCKET_ERROR -1

#define MAX_BUFFER_SIZE 4096
#define MAX_CLIENT_COUNT 10
#define SA struct sockaddr

#define MAP_HEIGHT 10
#define MAP_WIDTH 10
#define CELL_SIZE 64

void err_n_die(const char *fmt, ...);
bool check(int exception_to_check, const char *err_message);

#endif // !_SOCKETS_