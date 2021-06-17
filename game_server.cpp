#include "game_server.h"

void game_server::send_snake_data() {
    _server.send_data(_snake_game.get_snake_data());
}

void game_server::set_snake_direction() {
    _snake_game.set_snake_direction(_server.get_received_data());
}
