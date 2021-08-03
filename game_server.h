#pragma once
#include "servers.h"
#include "snake.h"
#include "snake_game.h"

class game_server : public Igame_server {
public:
    game_server(servers& servers, snake_game& snake_game);
    ~game_server();
    void start_game();
    void add_server_initial_data();
    void update_game(uint8_t byte_received) override;

private:
    void sleep_game_loop(size_t interval_ms, size_t measured_time_ms);
    void start_accepting_clients();

    servers& _servers;
    snake_game& _snake_game;
    uint8_t _data_received = 0;
    std::thread _snake_game_th;
    std::atomic<bool> _game_running{false};
};
