#pragma once
#include "server.h"
#include "snake.h"
#include "snake_game.h"

class game_server {
public:
    game_server(server& server, snake_game& snake_game) : _server(server), _snake_game(snake_game) {}
    ~game_server();
    void send_snake_data();
    void start_game();
    void set_snake_direction();

private:
    server& _server;
    snake_game& _snake_game;
    uint8_t _data_received = 0;
    std::thread _snake_game_th;
    std::atomic<bool> _game_running{false};
};
