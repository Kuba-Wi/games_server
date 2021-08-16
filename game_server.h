#pragma once
#include "servers.h"
#include "snake.h"
#include "snake_game.h"

class game_server : public Iservers_observer, public Igame_observer {
public:
    game_server(std::unique_ptr<servers>&& servers, std::unique_ptr<snake_game>&& snake_game);
    ~game_server();
    void start_game();
    void update_game(uint8_t byte_received);
    void update_snake_moved(const std::vector<int8_t>& snake_data);

private:
    void add_server_initial_data();
    void sleep_game_loop(size_t interval_ms, size_t measured_time_ms);
    void start_accepting_clients();

    std::unique_ptr<servers> _servers_ptr;
    std::unique_ptr<snake_game> _snake_game_ptr;
    uint8_t _data_received = 0;
    std::thread _snake_game_th;
    std::atomic<bool> _game_running{false};
};
