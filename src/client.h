#ifndef _client_H_
#define _client_H_

#include "server.h"

int init_client();

int send_message(int socketfd, const char *message);

int read_message(int socketfd, char *buffer);

int non_blocking_read(int socketfd, char *buffer);

int non_blocking_send(int socketfd, const char *message);

#endif // !_client_H_