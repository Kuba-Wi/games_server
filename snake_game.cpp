#include "snake_game.h"
#include <functional>

void snake_game::start_game() {
    _running = true;
    _timer.start_timer(std::bind(&snake::move, &_snake), _interval_ms);
}
