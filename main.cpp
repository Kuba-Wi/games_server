#include "game_server.h"
#include "servers.h"
#include "snake.h"
#include "snake_game.h"

int main() {
    snake_game sg(500, 10, 12);
    servers serv;
    game_server g_server(serv, sg);
    g_server.add_server_initial_data();
    g_server.start_game();

    while (true) {}
    
    return 0;
}
