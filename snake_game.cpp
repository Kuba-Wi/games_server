#include "snake_game.h"

void snake_game::start_game() {
    _timer.start_timer([&](){
        if (_snake.is_collision()) {
            return false;
        }
        if (_snake.is_food_eaten()) {
            _snake.add_snake_index();
            _snake.new_food();
        }
        _snake.move();
        return true;
        }, _interval_ms);
}
