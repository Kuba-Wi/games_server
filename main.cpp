#include "game_server.h"
#include "server.h"
#include "snake.h"
#include "snake_game.h"

int main() {
    snake_game sg(500);
    server serv;
    game_server g_server(serv, sg);
    g_server.receive_data();
    g_server.start_game();
    sg.set_snake_direction(2);

    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        g_server.send_snake_data();
    }

    return 0;
}
