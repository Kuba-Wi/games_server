#include "snake_game.h"

#include <mutex>

#include "game_server.h"

snake_game::snake_game(std::unique_ptr<timer<snake_game>>&& timer) :
    _timer_ptr(std::move(timer)) {}

void snake_game::attach_observer(game_server* observer) {
    std::lock_guard lg(_observer_mx);
    _server_observer = observer;
}

void snake_game::set_board_size(uint8_t height, uint8_t width) {
    if (snake::is_board_size_proper(height, width)) {
        _snake.set_board_size(height, width);
        _snake.reset_snake();
    }
}

void snake_game::notify_snake_moved() const {
    std::lock_guard lg(_observer_mx);
    if (_server_observer) {
        _server_observer->update_snake_moved(_snake.get_data());
    }
}

void snake_game::notify_game_finished() const {
    std::lock_guard lg(_observer_mx);
    if (_server_observer) {
        _server_observer->update_game_finished();
    }
}

void snake_game::restart_game(size_t interval_ms) {
    this->stop_game();

    if (_snake.is_board_size_set()) {
        _snake.reset_snake();
        _timer_ptr->start_timer(timer_function{*this}, interval_ms);
    }
}

void snake_game::change_time_interval(size_t new_interval_ms) {
    if (!_timer_ptr->is_started()) {
        return;
    }

    _timer_ptr->stop_timer();
    _timer_ptr->start_timer(timer_function{*this}, new_interval_ms);
}

void snake_game::stop_game() {
    _timer_ptr->stop_timer();
}

snake_game::~snake_game() {
    this->stop_game();
}

template <>
void timer_function<snake_game>::operator()() {
    if (_object._snake.is_food_eaten()) {
        _object._snake.add_snake_index();
        _object._snake.new_food();
    }
    _object._snake.move();
    _object.notify_snake_moved();
    if (_object._snake.is_collision()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        _object._snake.reset_snake();
        _object.notify_game_finished();
    }
}
