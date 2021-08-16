#include "game_server.h"
#include <chrono>

game_server::game_server(std::unique_ptr<servers>&& servers, std::unique_ptr<snake_game>&& snake_game) : 
        _servers_ptr(std::move(servers)), _snake_game_ptr(std::move(snake_game)) {

    _servers_ptr->attach_observer(this);
    _snake_game_ptr->attach_observer(this);
    this->add_server_initial_data();
}

void game_server::start_game() {
    using namespace std::chrono;
    constexpr size_t time_fraction = 10;
    size_t time_interval_ms = _snake_game_ptr->get_snake_move_time_ms() / time_fraction;

    _servers_ptr->start_servers();
    _snake_game_ptr->start_new_game();
    _game_running = true;
    _snake_game_th = std::thread{[&, time_interval_ms](){
        time_point<steady_clock> start;
        time_point<steady_clock> end;
        while (_game_running) {
            start = steady_clock::now();

            if (!_snake_game_ptr->is_game_in_progress()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                _servers_ptr->change_receiving_server();
                _snake_game_ptr->start_new_game();
            }

            end = steady_clock::now();
            this->sleep_game_loop(time_interval_ms, duration_cast<milliseconds>(end - start).count());
        }
    }};
}

void game_server::add_server_initial_data() {
    std::vector<int8_t> board_dimensions{static_cast<int8_t>(_snake_game_ptr->get_board_height()), 
                                         static_cast<int8_t>(_snake_game_ptr->get_board_width())};

    _servers_ptr->set_initial_data(board_dimensions);
}

void game_server::sleep_game_loop(size_t interval_ms, size_t measured_time_ms) {
    if (measured_time_ms < interval_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms - measured_time_ms));
    }
}

void game_server::update_game(uint8_t byte_received) {
    _snake_game_ptr->set_snake_direction(byte_received);
}

void game_server::update_snake_moved(const std::vector<int8_t>& snake_data) {
    _servers_ptr->send_data(snake_data);
}

game_server::~game_server() {
    _game_running = false;
    if (_snake_game_th.joinable()) {
        _snake_game_th.join();
    }
}
