#pragma once

#include "snake.h"
#include "timer.h"

#include <condition_variable>

class snake_game {
public:
    snake_game(size_t interval_ms) : _interval_ms(interval_ms) {}
    ~snake_game();
    void start_game();
    auto get_snake_data() {
        return _snake.get_data();
    }
    void set_snake_direction(uint8_t direction) { _snake.set_current_direction(static_cast<move_direction>(direction)); }
    size_t get_snake_move_time_ms() const { return _interval_ms; }

private:
    void start_snake();

    snake _snake;
    timer _timer;
    size_t _interval_ms;
    std::thread _game_end_th;
    std::condition_variable _game_end_cv;
    std::atomic<bool> _game_in_progress{false};
};
