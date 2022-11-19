// #include "window.h"
#include "./game/game.h"

// int main(int argc, char **argv)
// {
//     // parse the argument to decide if we are a client or a server

//     if (argc != 2)
//         err_n_die("Usage: %s <client|server>", argv[0]);

//     if (strcmp(argv[1], "client") == 0)
//         init_window();
//     else if (strcmp(argv[1], "server") == 0)
//         init_server();
//     else
//         err_n_die("Usage: %s <client|server>", argv[0]);

//     return 0;
// }

int main(int argc, char *argv[])
{
    Game game;
    init_game(&game);

    add_player(&game);
    add_player(&game);
    add_player(&game);
    add_player(&game);

    // add_player(&game);

    while (game.is_running)
    {
        process_input(&game);
        update(&game);
        render(&game);
    }

    destroy_game(&game);

    return 0;
}
