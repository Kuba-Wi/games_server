#pragma once

#include <memory>

#include "snake.h"
#include "timer.h"

class game_server;

class snake_game {
    friend class timer_function<snake_game>;
public:
    snake_game(std::unique_ptr<Itimer<snake_game>>&& timer, size_t interval_ms, uint8_t height, uint8_t width);
    virtual ~snake_game();
    virtual void attach_observer(game_server* observer);
    virtual void start_new_game();
    virtual void set_snake_direction(uint8_t direction) { _snake.set_current_direction(static_cast<move_direction>(direction)); }

    auto get_snake_data() { return _snake.get_data(); }
    size_t get_snake_move_time_ms() const { return _interval_ms; }
    uint8_t get_board_height() const { return _snake.get_board_height(); }
    uint8_t get_board_width() const { return _snake.get_board_width(); }

private:
    void start_snake();
    void notify_snake_moved() const;
    void notify_game_finished() const;

    std::unique_ptr<Itimer<snake_game>> _timer_ptr;
    snake _snake;
    size_t _interval_ms;

    game_server* _server_observer = nullptr;
    mutable std::mutex _observer_mx;
};
