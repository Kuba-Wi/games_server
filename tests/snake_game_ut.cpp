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

TEST_F(snakeGameTest, setBoardSizeShouldSetBoardSizesAndResetSnake) {
    EXPECT_CALL(*mock, stop_timer()).Times(2);
    EXPECT_CALL(*mock, get_current_interval_ms()).WillOnce(Return(std::nullopt));
    snake_game s_game{std::move(mock)};
    s_game.set_board_size(height, width);

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

TEST_F(snakeGameTest, setBoardSizeShouldSetBoardSizesAndRestartTimerWhenTimerIsStarted) {
    EXPECT_CALL(*mock, stop_timer()).Times(3);
    EXPECT_CALL(*mock, get_current_interval_ms()).WillOnce(Return(interval_ms));
    EXPECT_CALL(*mock, restart_timer(_, interval_ms));
    snake_game s_game{std::move(mock)};
    s_game.restart_game(interval_ms);
    s_game.set_board_size(height, width);
}

TEST_F(snakeGameTest, setBoardSizeShouldDoNothingWhenSizeIsNotProper) {
    constexpr size_t wrong_height = 0;
    constexpr size_t wrong_width = 0;
    EXPECT_CALL(*mock, stop_timer()).Times(1);
    snake_game s_game{std::move(mock)};
    s_game.set_board_size(wrong_height, wrong_width);
}

TEST_F(snakeGameTest, restartGameShouldStopAndStartTimer) {
    EXPECT_CALL(*mock, get_current_interval_ms()).WillOnce(Return(std::nullopt));
    EXPECT_CALL(*mock, restart_timer(_, interval_ms));
    EXPECT_CALL(*mock, stop_timer()).Times(3);
    snake_game s_game{std::move(mock)};
    s_game.set_board_size(height, width);
    s_game.restart_game(interval_ms);
}

TEST_F(snakeGameTest, multipleStartNewGameShouldStartTimerMultipleTimes) {
    constexpr size_t start_calls_count = 10;
    constexpr size_t stop_calls_count = start_calls_count + 2;

    EXPECT_CALL(*mock, get_current_interval_ms()).WillOnce(Return(std::nullopt));
    EXPECT_CALL(*mock, restart_timer(_, interval_ms)).Times(start_calls_count);
    EXPECT_CALL(*mock, stop_timer()).Times(stop_calls_count);
    snake_game s_game{std::move(mock)};
    s_game.set_board_size(height, width);

    for (size_t i = 0; i < start_calls_count; ++i) {
        s_game.restart_game(interval_ms);
    }
}

TEST_F(snakeGameTest, snakeShouldNotBeStartedWhenBoardSizesAreNotSet) {
    EXPECT_CALL(*mock, restart_timer(_, _)).Times(0);
    EXPECT_CALL(*mock, stop_timer()).Times(2);
    snake_game s_game{std::move(mock)};
    s_game.restart_game(interval_ms);
}

TEST_F(snakeGameTest, snakeShouldBeStoppedAndStartedAfterResizingBoard) {
    EXPECT_CALL(*mock, get_current_interval_ms())
        .WillOnce(Return(std::nullopt))
        .WillOnce(Return(interval_ms));
    EXPECT_CALL(*mock, restart_timer(_, interval_ms)).Times(2);
    EXPECT_CALL(*mock, stop_timer()).Times(4);
    snake_game s_game{std::move(mock)};
    s_game.set_board_size(height, width);
    s_game.restart_game(interval_ms);

    s_game.set_board_size(height, width);
}

TEST_F(snakeGameTest, snakeShouldBeRestartedAfterChangeOfTimeInterval) {
    constexpr size_t new_interval_ms = 2;

    EXPECT_CALL(*mock, get_current_interval_ms()).WillOnce(Return(std::nullopt));
    EXPECT_CALL(*mock, is_started()).WillOnce(Return(true));
    EXPECT_CALL(*mock, restart_timer(_, interval_ms));
    EXPECT_CALL(*mock, restart_timer(_, new_interval_ms));
    EXPECT_CALL(*mock, stop_timer()).Times(3);
    snake_game s_game{std::move(mock)};
    s_game.set_board_size(height, width);
    s_game.restart_game(interval_ms);

    s_game.change_time_interval(new_interval_ms);
}
