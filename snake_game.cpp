#include "snake_game.h"
#include <mutex>

void snake_game::notify_snake_moved() {
    if (_server_observer) {
        _server_observer->update_snake_moved(_snake.get_data());
    }
}

void snake_game::notify_game_finished() {
    if (_server_observer) {
        _server_observer->update_game_finished();
    }
}

void snake_game::start_new_game() {
    _timer.stop_timer();
    _snake.reset_snake();

    this->start_snake();
}

void snake_game::start_snake() {
        _timer.start_timer([&](){
            if (_snake.is_food_eaten()) {
            _snake.add_snake_index();
            _snake.new_food();
        }
        _snake.move();
        this->notify_snake_moved();
        if (_snake.is_collision()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            _snake.reset_snake();
            this->notify_game_finished();
        }
    }, _interval_ms);
}

snake_game::~snake_game() {
    _timer.stop_timer();
}
