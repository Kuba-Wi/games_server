#include "game_server.h"
#include "server.h"
#include "snake.h"
#include "snake_game.h"

int main() {
    snake_game sg(500);
    server serv;
    game_server g_server(serv, sg);
    g_server.start_game();

    while (true) {}
    
    return 0;
}
