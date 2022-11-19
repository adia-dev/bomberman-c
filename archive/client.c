#include "client.h"

int init_client()
{

    char name[20];
    printf("Enter your name: ");
    fgets(name, 20, stdin);
    name[strlen(name) - 1] = '\0';

    int socketfd;
    struct sockaddr_in clientaddr, serveraddr;
    size_t sendbytes;
    char sendline[MAX_BUFFER_SIZE];
    char recvline[MAX_BUFFER_SIZE];

    check((socketfd = socket(AF_INET, SOCK_STREAM, 0)), "Could not create the socket");

    bzero(&clientaddr, sizeof(clientaddr));
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_addr.s_addr = INADDR_ANY;
    clientaddr.sin_port = htons(SERVER_PORT);

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serveraddr.sin_port = htons(SERVER_PORT);

    check(connect(socketfd, (SA *)&serveraddr, sizeof(serveraddr)), "Could not connect to the server");

    send(socketfd, name, strlen(name), 0);

    printf("Client %s connected to the server\n\n", name);

    return socketfd;
}

int send_message(int socketfd, const char *message)
{
    return send(socketfd, message, strlen(message), 0);
}

int read_message(int socketfd, char *buffer)
{
    return read(socketfd, buffer, MAX_BUFFER_SIZE - 1);
}

int non_blocking_read(int socketfd, char *buffer)
{
    int flags = fcntl(socketfd, F_GETFL, 0);
    fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);
    return read(socketfd, buffer, MAX_BUFFER_SIZE - 1);
}

int non_blocking_send(int socketfd, const char *message)
{
    int flags = fcntl(socketfd, F_GETFL, 0);
    fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);
    return send(socketfd, message, strlen(message), 0);
}
