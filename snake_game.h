#pragma once

#include "snake.h"
#include "timer.h"

class snake_game {
public:
    snake_game(size_t interval_ms) : _interval_ms(interval_ms) {}
    ~snake_game();
    void start_game();
    auto get_snake_data() {
        return _snake.get_data();
    }
    void set_snake_direction(uint8_t direction) { _snake.set_current_direction(static_cast<move_direction>(direction)); }

private:
    snake _snake;
    timer _timer;
    size_t _interval_ms;
    std::thread _game_end_th;
    std::atomic<bool> _game_in_progress{false};
};
