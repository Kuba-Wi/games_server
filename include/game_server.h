#pragma once

#include "privileged_connection.h"
#include "servers.h"
#include "snake.h"
#include "snake_game.h"

class game_server  {
public:
    game_server(std::unique_ptr<servers>&& servers, 
                std::unique_ptr<snake_game>&& snake_game, 
                std::unique_ptr<privileged_connection>&& p_connection);

    void restart_game(size_t interval_ms);
    void stop_game();
    void change_snake_move_time(size_t move_time_ms);
    void update_game(uint8_t byte_received);
    void update_snake_moved(const std::vector<int8_t>& snake_data);
    void update_game_finished();

    size_t get_clients_count() const;
    void update_board_size(uint8_t height, uint8_t width);

    void update_clients_count_changed(size_t clients_count);

private:
    std::unique_ptr<servers> _servers_ptr;
    std::unique_ptr<snake_game> _snake_game_ptr;
    std::unique_ptr<privileged_connection> _priv_connection;
};
