#pragma once
#include "server.h"
#include "snake.h"

class game_server {
public:
    game_server(server& server, snake& snake) : _server(server), _snake(snake) {}
    void send_snake_data();
    void receive_data();

private:
    server& _server;
    snake& _snake;
};
