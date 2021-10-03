#pragma once

#include <memory>

#include "snake.h"
#include "test_iface.h"
#include "timer.h"

class game_server;

class snake_game {
    friend class timer_function<snake_game>;
public:
    snake_game(std::unique_ptr<timer<snake_game>>&& timer);
    TEST_IFACE ~snake_game();
    TEST_IFACE void attach_observer(game_server* observer);

    void set_board_size(uint8_t height, uint8_t width);
    TEST_IFACE void restart_game(size_t interval_ms);
    void stop_game();
    TEST_IFACE void set_snake_direction(uint8_t direction) { _snake.set_current_direction(static_cast<move_direction>(direction)); }

    auto get_snake_data() { return _snake.get_data(); }

private:
    void notify_snake_moved() const;
    void notify_game_finished() const;

    std::unique_ptr<timer<snake_game>> _timer_ptr;
    snake _snake;

    game_server* _server_observer = nullptr;
    mutable std::mutex _observer_mx;
};
