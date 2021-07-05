#include "game_server.h"

void game_server::send_snake_data() {
    _server.send_data(_snake_game.get_snake_data());
}

void game_server::set_snake_direction() {
    _snake_game.set_snake_direction(_server.get_received_data());
}

void game_server::start_game() { 
    _snake_game.start_game();
    _game_running = true;
    _snake_game_th = std::thread{[&](){
        while (_game_running) {
            this->send_snake_data();
            this->set_snake_direction();
        }
    }};
}

game_server::~game_server() {
    _game_running = false;
    if (_snake_game_th.joinable()) {
        _snake_game_th.join();
    }
}
