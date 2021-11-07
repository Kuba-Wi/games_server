#pragma once

#include <gmock/gmock.h>

#include "snake_game.h"

class snake_game_mock : public snake_game {
public:
    snake_game_mock(std::unique_ptr<timer<snake_game>>&& timer) :
                    snake_game(std::move(timer)) {}
    
    MOCK_METHOD(void, attach_observer, (game_server*), (override));
    MOCK_METHOD(void, restart_game, (size_t), (override));
    MOCK_METHOD(void, set_snake_direction, (uint8_t), (override));
};
