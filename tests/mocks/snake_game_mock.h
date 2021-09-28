#pragma once

#include <gmock/gmock.h>

#include "snake_game.h"

class snake_game_mock : public snake_game {
public:
    snake_game_mock(std::unique_ptr<timer<snake_game>>&& timer, size_t interval_ms, uint8_t height, uint8_t width) :
                    snake_game(std::move(timer), interval_ms, height, width) {}
    
    MOCK_METHOD(void, attach_observer, (game_server*), (override));
    MOCK_METHOD(void, restart_game, (), (override));
    MOCK_METHOD(void, set_snake_direction, (uint8_t), (override));
};
