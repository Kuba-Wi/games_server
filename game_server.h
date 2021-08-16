#pragma once
#include "servers.h"
#include "snake.h"
#include "snake_game.h"

class game_server : public Iservers_observer, public Igame_observer {
public:
    game_server(std::unique_ptr<servers>&& servers, std::unique_ptr<snake_game>&& snake_game);
    void start_game();
    void update_game(uint8_t byte_received) override;
    void update_snake_moved(const std::vector<int8_t>& snake_data) override;
    void update_game_finished() override;

private:
    void add_server_initial_data();
    void start_accepting_clients();

    std::unique_ptr<servers> _servers_ptr;
    std::unique_ptr<snake_game> _snake_game_ptr;
};
