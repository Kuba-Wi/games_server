#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mocks/timer_mock.h"

#include "snake_game.h"

constexpr size_t interval_ms = 1;
constexpr size_t height = 10;
constexpr size_t width = 10;

using namespace ::testing;
using TimerMock = NaggyMock<timer_mock<snake_game>>;

struct snakeGameTest : public Test {
    std::unique_ptr<TimerMock> mock = std::make_unique<TimerMock>();
};

TEST_F(snakeGameTest, constructorShouldSetBoardSizesTimeIntervalAndSnake) {
    EXPECT_CALL(*mock, stop_timer()).Times(1);
    snake_game s_game{std::move(mock), interval_ms, height, width};
    ASSERT_EQ(s_game.get_board_height(), height);
    ASSERT_EQ(s_game.get_board_width(), width);
    ASSERT_EQ(s_game.get_snake_move_time_ms(), interval_ms);

    constexpr size_t data_size = 6;
    auto data = s_game.get_snake_data();
    ASSERT_EQ(data.size(), data_size);
    std::pair head{data[0], data[1]};
    std::pair tail{data[2], data[3]};
    std::pair food{data[4], data[5]};
    ASSERT_EQ(head.first, tail.first - 1);
    ASSERT_EQ(head.second, tail.second);
    ASSERT_NE(food, head);
    ASSERT_NE(food, tail);
}

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
