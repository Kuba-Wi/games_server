#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mocks/timer_mock.h"

#include "snake_game.h"

constexpr size_t interval_ms = 1;
constexpr size_t height = 10;
constexpr size_t width = 10;

using namespace ::testing;
using TimerMock = NaggyMock<timer_mock>;

struct snakeGameTest : public Test {
    std::unique_ptr<TimerMock> mock = std::make_unique<TimerMock>();
};

TEST_F(snakeGameTest, startNewGameShouldStopAndStartTimer) {
    EXPECT_CALL(*mock, start_timer(_, interval_ms)).Times(1);
    EXPECT_CALL(*mock, stop_timer()).Times(2);
    snake_game s_game{std::move(mock), interval_ms, height, width};
    s_game.start_new_game();
}

TEST_F(snakeGameTest, multipleStartNewGameShouldStartTimerMultipleTimes) {
    constexpr size_t start_calls_count = 10;
    constexpr size_t stop_calls_count = start_calls_count + 1;
    EXPECT_CALL(*mock, start_timer(_, interval_ms)).Times(start_calls_count);
    EXPECT_CALL(*mock, stop_timer()).Times(stop_calls_count);
    snake_game s_game{std::move(mock), interval_ms, height, width};
    for (size_t i = 0; i < start_calls_count; ++i) {
        s_game.start_new_game();
    }
}
