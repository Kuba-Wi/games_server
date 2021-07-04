#include "snake_game.h"

void snake_game::start_game() {
    _game_in_progress = true;
    _game_end_th = std::thread{
        [&](){
            while (_game_in_progress) {
                if (_snake.is_collision()) {
                    _game_in_progress = false;
                }
            }
            _timer.stop_timer();
        }
    };
    _timer.start_timer([&](){
        if (_snake.is_food_eaten()) {
            _snake.add_snake_index();
            _snake.new_food();
        }
        _snake.move();
        }, _interval_ms);
}

snake_game::~snake_game() {
    _game_in_progress = false;
    if (_game_end_th.joinable()) {
        _game_end_th.join();
    }
}
