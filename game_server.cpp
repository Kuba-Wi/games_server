#include "game_server.h"

void game_server::send_snake_data() {
    auto data_body = _snake.snake_index_data();
    auto data_food = _snake.food_index_data();
    std::vector<decltype(data_body)> data{data_body, data_food};
    _server.send_data(data);
}

void game_server::receive_data() {
    _server.receive_data();
}
