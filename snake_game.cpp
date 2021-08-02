#include "snake_game.h"
#include <mutex>

void snake_game::start_new_game() {
    this->stop_current_game();
    _snake.reset_snake();

    _game_in_progress = true;
    _game_end_th = std::thread{
        [&](){
            std::mutex mx;
            std::unique_lock ul(mx);
            _game_end_cv.wait(ul, [&](){
                return !_game_in_progress;
            });
            _timer.stop_timer();
        }
    };
    this->start_snake();
}

void snake_game::start_snake() {
    _timer.start_timer([&](){
            if (_snake.is_food_eaten()) {
                _snake.add_snake_index();
                _snake.new_food();
            }
            _snake.move();
            if (_snake.is_collision()) {
                _game_in_progress = false;
                _game_end_cv.notify_all();
            }
        }, _interval_ms);
}

void snake_game::stop_current_game() {
    _game_in_progress = false;
    _game_end_cv.notify_all();
    if (_game_end_th.joinable()) {
        _game_end_th.join();
    }
}

snake_game::~snake_game() {
    this->stop_current_game();
}
