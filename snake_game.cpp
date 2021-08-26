#include "snake_game.h"
#include <mutex>

snake_game::snake_game(std::unique_ptr<Itimer>&& timer, size_t interval_ms, uint8_t height, uint8_t width) :
    _timer_ptr(std::move(timer)),
    _snake(height, width), 
    _interval_ms(interval_ms) {}

void snake_game::notify_snake_moved() const {
    if (_server_observer) {
        _server_observer->update_snake_moved(_snake.get_data());
    }
}

void snake_game::notify_game_finished() const {
    if (_server_observer) {
        _server_observer->update_game_finished();
    }
}

void snake_game::start_new_game() {
    _timer_ptr->stop_timer();
    _snake.reset_snake();

    this->start_snake();
}

void snake_game::start_snake() {
        _timer_ptr->start_timer([&](){
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
    _timer_ptr->stop_timer();
}
