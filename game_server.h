#pragma once
#include "servers.h"
#include "snake.h"
#include "snake_game.h"

class game_server {
public:
    game_server(servers& servers, snake_game& snake_game) : _servers(servers), _snake_game(snake_game) {}
    ~game_server();
    void start_game();

private:
    inline
    void sleep_game_loop(size_t interval_ms, size_t measured_time_ms);

    servers& _servers;
    snake_game& _snake_game;
    uint8_t _data_received = 0;
    std::thread _snake_game_th;
    std::atomic<bool> _game_running{false};
};
