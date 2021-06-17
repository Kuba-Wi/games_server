#include "game_server.h"
#include "server.h"
#include "snake.h"
#include "snake_game.h"

int main() {
    snake_game sg(500);
    server serv;
    game_server g_server(serv, sg);
    g_server.start_game();

    while (true) {
        g_server.send_snake_data();
        g_server.set_snake_direction();
    }

    return 0;
}
