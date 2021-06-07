#include "game_server.h"
#include "server.h"
#include "snake.h"

int main() {
    server serv;
    snake snak;
    game_server g_server(serv, snak);
    g_server.receive_data();
    g_server.send_snake_data();

    return 0;
}
