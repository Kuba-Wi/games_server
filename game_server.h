#pragma once
#include "server.h"
#include "snake.h"
#include "snake_game.h"

class game_server {
public:
    game_server(server& server, snake_game& snake_game) : _server(server), _snake_game(snake_game) {}
    void send_snake_data();
    void receive_data();
    void start_game() { _snake_game.start_game(); }

private:
    server& _server;
    snake_game& _snake_game;
};
