#pragma once

#include "snake.h"
#include "timer.h"

class Igame_observer {
public:
    virtual ~Igame_observer() = default;
    virtual void update_snake_moved(const std::vector<int8_t>& snake_data) = 0;
    virtual void update_game_finished() = 0;
};

class snake_game {
public:
    snake_game(size_t interval_ms, uint8_t height, uint8_t width) : _snake(height, width), _interval_ms(interval_ms) {}
    ~snake_game();
    void attach_observer(Igame_observer* observer) { _server_observer = observer; }
    void notify_snake_moved();
    void notify_game_finished();
    void start_new_game();
    void set_snake_direction(uint8_t direction) { _snake.set_current_direction(static_cast<move_direction>(direction)); }

    auto get_snake_data() { return _snake.get_data(); }
    size_t get_snake_move_time_ms() const { return _interval_ms; }
    uint8_t get_board_height() const { return _snake.get_board_height(); }
    uint8_t get_board_width() const { return _snake.get_board_width(); }

private:
    void start_snake();

    snake _snake;
    timer _timer;
    size_t _interval_ms;

    Igame_observer* _server_observer = nullptr;
};
