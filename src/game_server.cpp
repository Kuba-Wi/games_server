#include "game_server.h"

game_server::game_server(std::unique_ptr<servers>&& servers, std::unique_ptr<snake_game>&& snake_game) : 
        _servers_ptr(std::move(servers)), _snake_game_ptr(std::move(snake_game)) {

    _servers_ptr->attach_observer(this);
    _snake_game_ptr->attach_observer(this);
    this->add_server_initial_data();
}

void game_server::start_game() {
    _snake_game_ptr->start_new_game();
}

void game_server::add_server_initial_data() {
    std::vector<int8_t> board_dimensions{static_cast<int8_t>(_snake_game_ptr->get_board_height()), 
                                         static_cast<int8_t>(_snake_game_ptr->get_board_width())};

    _servers_ptr->set_initial_data(board_dimensions);
}

void game_server::update_game(uint8_t byte_received) {
    _snake_game_ptr->set_snake_direction(byte_received);
}

void game_server::update_snake_moved(const std::vector<int8_t>& snake_data) {
    _servers_ptr->send_data(snake_data);
}

void game_server::update_game_finished() {
    _servers_ptr->change_receiving_server();
}
