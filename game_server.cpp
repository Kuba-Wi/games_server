#include "game_server.h"
#include <chrono>

void game_server::start_game() {
    using namespace std::chrono;
    constexpr size_t time_fraction = 10;
    size_t time_interval_ms = _snake_game.get_snake_move_time_ms() / time_fraction;
    time_point<steady_clock> start;
    time_point<steady_clock> end;
    std::optional<uint8_t> opt_data_received;

    _snake_game.start_game();
    _game_running = true;
    _snake_game_th = std::thread{[&](){
        while (_game_running) {
            start = steady_clock::now();

            _servers.send_data(_snake_game.get_snake_data());
            _servers.remove_disconnected_serv();
            opt_data_received = _servers.get_data_received();
            if (opt_data_received) {
                _snake_game.set_snake_direction(opt_data_received.value());
            }

            end = steady_clock::now();
            this->sleep_game_loop(time_interval_ms, duration_cast<milliseconds>(end - start).count());
        }
    }};
}

void game_server::add_server_initial_data() {
    std::vector<int8_t> board_dimensions{static_cast<int8_t>(_snake_game.get_board_height()), 
                                         static_cast<int8_t>(_snake_game.get_board_width())};

    _servers.set_initial_data(board_dimensions);
}

void game_server::sleep_game_loop(size_t interval_ms, size_t measured_time_ms) {
    if (measured_time_ms < interval_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms - measured_time_ms));
    }
}

game_server::~game_server() {
    _game_running = false;
    if (_snake_game_th.joinable()) {
        _snake_game_th.join();
    }
}
