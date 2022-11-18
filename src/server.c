#include "server.h"

int init_server()
{
    int opt = true;
    int master_socket, addrlen, new_socket, client_socket[30],
        max_clients = 30, activity, i, valread, sd;
    int max_sd;
    int client_count = 0;
    struct client clients[30];
    struct sockaddr_in address;

    char buffer[MAX_BUFFER_SIZE]; // data buffer of 1K

    // set of socket descriptors
    fd_set readfds, writefds;

    char map[MAP_HEIGHT][MAP_WIDTH] = {
        {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'},
        {'x', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'x'},
        {'x', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'x'},
        {'x', ' ', ' ', '1', ' ', ' ', ' ', ' ', ' ', 'x'},
        {'x', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'x'},
        {'x', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'x'},
        {'x', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'x'},
        {'x', ' ', ' ', ' ', ' ', ' ', '2', ' ', ' ', 'x'},
        {'x', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'x'},
        {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'}};

    // a message
    char *message = "ECHO Daemon v1.0 \r\n";

    char *playerCharacters[2] = {"1", "2"};

    Coords playerCoords[2];
    playerCoords[0] = get_player_coords(map, '1');
    playerCoords[1] = get_player_coords(map, '2');

    // initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
        clients[i] = (struct client){0, NULL, (struct sockaddr_in){0}};
    }

    // create a master socket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // set master socket to allow multiple connections ,
    // this is just a good habit, it will work without this
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                   sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SERVER_PORT);

    // bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", SERVER_PORT);

    // try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while (true)
    {
        // clear the socket set
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);

        // add master socket to set
        FD_SET(master_socket, &readfds);
        FD_SET(master_socket, &writefds);
        max_sd = master_socket;

        // add child sockets to set
        for (i = 0; i < max_clients; i++)
        {
            // socket descriptor
            // sd = client_socket[i];
            sd = clients[i].socketfd;

            // if valid socket descriptor then add to read list
            if (sd > 0)
            {
                FD_SET(sd, &readfds);
                FD_SET(sd, &writefds);
            }

            // highest file descriptor number, need it for the select function
            if (sd > max_sd)
                max_sd = sd;
        }

        // wait for an activity on one of the sockets , timeout is NULL ,
        // so wait indefinitely
        activity = select(max_sd + 1, &readfds, &writefds, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            printf("select error");
        }

        // If something happened on the master socket ,
        // then its an incoming connection
        if (FD_ISSET(master_socket, &readfds))
        {
            if ((new_socket = accept(master_socket,
                                     (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n ", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // send new connection greeting message
            if (send(new_socket, playerCharacters[client_count++], 2, 0) != 2)
            {
                perror("send");
            }

            puts("Welcome message sent successfully");

            // add new socket to array of sockets
            for (i = 0; i < max_clients; i++)
            {
                // if position is empty
                if (clients[i].socketfd == 0)
                {
                    clients[i].socketfd = new_socket;
                    clients[i].address = address;
                    printf("Adding to list of sockets as %d\n", i);

                    memset(buffer, 0, MAX_BUFFER_SIZE);
                    if (read(new_socket, buffer, MAX_BUFFER_SIZE) > 0)
                    {
                        clients[i].name = (char *)malloc(strlen(buffer) + 1);
                        clients[i].name[strlen(buffer)] = '\0';
                        strcpy(clients[i].name, buffer);
                    }

                    break;
                }
            }
        }

        // else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)
        {
            sd = clients[i].socketfd;

            if (FD_ISSET(sd, &readfds))
            {
                // Check if it was for closing , and also read the
                // incoming message
                if ((valread = read(sd, buffer, MAX_BUFFER_SIZE - 1)) == 0)
                {
                    // Somebody disconnected , get his details and print
                    getpeername(sd, (struct sockaddr *)&address,
                                (socklen_t *)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n",
                           inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    // Close the socket and mark as 0 in list for reuse
                    close(sd);
                    free(clients[i].name);
                    clients[i] = (struct client){0, NULL, (struct sockaddr_in){0}};
                    client_count--;
                }

                // Echo back the message that came in
                else
                {
                    // set the string terminating NULL byte on the end
                    // of the data read
                    // memset(buffer, 0, MAX_BUFFER_SIZE);
                    // buffer[valread] = '\0';
                    //
                    // send(sd, buffer, strlen(buffer), 0);

                    // send to all
                    printf("%s: %s\n", clients[i].name, buffer);

                    printf("Move from player: %s, %d\n", playerCharacters[i], i);

                    playerCoords[i] = maybe_move_cell(playerCoords[i], map, playerCharacters[i][0], buffer[0]);

                    fflush(stdout);
                    for (int j = 0; j < max_clients; j++)
                    {
                        if (clients[j].socketfd > 0)
                        {
                            buffer[valread] = '\0';
                            // send the name of the sender with the message
                            char *msg = (char *)malloc(strlen(clients[i].name) + strlen(buffer) + 2);
                            strcpy(msg, clients[i].name);
                            strcat(msg, ": ");
                            strcat(msg, buffer);
                            // send(clients[j].socketfd, msg, strlen(msg), 0);

                            char *map_message = map_to_string(map);
                            send(clients[j].socketfd, map_message, strlen(map_message), 0);

                            free(msg);
                            free(map_message);
                        }
                    }
                    memset(buffer, 0, MAX_BUFFER_SIZE);
                }
            }
        }
    }

    return 0;
}

char *map_to_string(char map[MAP_HEIGHT][MAP_WIDTH])
{
    // the map is MAP_HEIGHT * MAP_WIDTH and will be converted to a string separated by newlines each row

    char *map_message = (char *)malloc(MAP_HEIGHT * MAP_WIDTH + MAP_HEIGHT + 1);
    map_message[MAP_HEIGHT * MAP_WIDTH + MAP_HEIGHT] = '\0';

    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            map_message[i * MAP_WIDTH + j + i] = map[i][j];
        }
        map_message[i * MAP_WIDTH + MAP_WIDTH + i] = '\n';
    }

    return map_message;
}

char **string_to_map(const char *string)
{
    // the map is MAP_HEIGHT * MAP_WIDTH and will be converted to a string separated by newlines each row

    char **map = (char **)malloc(MAP_HEIGHT * sizeof(char *));
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        map[i] = (char *)malloc(MAP_WIDTH * sizeof(char));
    }

    int i = 0;
    int j = 0;
    int k = 0;
    while (string[k] != '\0')
    {
        if (string[k] == '\n')
        {
            i++;
            j = 0;
        }
        else
        {
            map[i][j] = string[k];
            j++;
        }
        k++;
    }

    return map;
}

Coords get_player_coords(char map[MAP_HEIGHT][MAP_WIDTH], char player)
{
    Coords coords = {0, 0};
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        for (int j = 0; j < MAP_WIDTH; j++)
        {
            if (map[i][j] == player)
            {
                coords.x = j;
                coords.y = i;
                return coords;
            }
        }
    }
    return coords;
}

Coords maybe_move_cell(Coords coords, char map[MAP_HEIGHT][MAP_WIDTH], char player, char direction)
{
    Coords new_coords = coords;
    switch (direction)
    {
    case 'w':
        new_coords.y--;
        break;
    case 'a':
        new_coords.x--;
        break;
    case 's':
        new_coords.y++;
        break;
    case 'd':
        new_coords.x++;
        break;
    default:
        break;
    }

    if (new_coords.x < 0 || new_coords.x >= MAP_WIDTH || new_coords.y < 0 || new_coords.y >= MAP_HEIGHT)
    {
        return coords;
    }

    if (map[new_coords.y][new_coords.x] == ' ')
    {
        map[new_coords.y][new_coords.x] = player;
        map[coords.y][coords.x] = ' ';
        return new_coords;
    }

    return coords;
}