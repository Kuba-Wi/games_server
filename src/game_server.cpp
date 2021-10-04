#include "game_server.h"

game_server::game_server(std::unique_ptr<servers>&& servers, 
                         std::unique_ptr<snake_game>&& snake_game, 
                         std::unique_ptr<privileged_connection>&& p_connection) : 
        _servers_ptr(std::move(servers)), 
        _snake_game_ptr(std::move(snake_game)),
        _priv_connection(std::move(p_connection)) {

    _servers_ptr->attach_observer(this);
    _snake_game_ptr->attach_observer(this);
    _priv_connection->attach_observer(this);

    _servers_ptr->accept_connections();
}

void game_server::restart_game(size_t interval_ms) {
    _snake_game_ptr->restart_game(interval_ms);
}

void game_server::stop_game() {
    _snake_game_ptr->stop_game();
}

void game_server::change_snake_move_time(size_t move_time_ms) {
    _snake_game_ptr->change_time_interval(move_time_ms);
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

size_t game_server::get_clients_count() const {
    return _servers_ptr->get_clients_count();
}

void game_server::update_board_size(uint8_t height, uint8_t width) {
    if (height != 0 && width != 0) {
        _snake_game_ptr->set_board_size(height, width);
        _servers_ptr->update_initial_data({static_cast<int8_t>(height), static_cast<int8_t>(width)});
    }
}

void game_server::update_clients_count_changed(size_t clients_count) {
    _priv_connection->send_clients_count(clients_count);
}
