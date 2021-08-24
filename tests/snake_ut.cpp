#include <gtest/gtest.h>

#include "snake.h"

constexpr size_t height = 8;
constexpr size_t width = 6;

struct snakeTest : public ::testing::Test {
    snakeTest() : sn(height, width) {}

    snake sn;
};

TEST_F(snakeTest, constructorShouldSetBoardSize) {
    ASSERT_EQ(sn.get_board_height(), height);
    ASSERT_EQ(sn.get_board_width(), width);
}

void check_initial_data(const std::vector<int8_t>& data) {
    constexpr size_t initial_data_size = 6;

    ASSERT_EQ(data.size(), initial_data_size);
    std::pair<int8_t, int8_t> snake_head{data[0], data[1]};
    std::pair<int8_t, int8_t> snake_tail{data[2], data[3]};
    std::pair<int8_t, int8_t> food_index{data[4], data[5]};

    ASSERT_EQ(snake_head.first, height / 2);
    ASSERT_EQ(snake_head.second, width / 2);
    ASSERT_EQ(snake_tail.first, height / 2 + 1);
    ASSERT_EQ(snake_tail.second, width / 2);

    ASSERT_NE(food_index, snake_head);
    ASSERT_NE(food_index, snake_tail);
}

TEST_F(snakeTest, getDataShouldReturnSnakeData) {
    auto data = sn.get_data();
    check_initial_data(data);
}

TEST_F(snakeTest, resetSnakeSchouldGiveInitialData) {
    sn.reset_snake();
    auto data = sn.get_data();
    check_initial_data(data);
}

TEST_F(snakeTest, addSnakeIndexShouldAddSnakeTail) {
    auto data_before = sn.get_data();
    sn.add_snake_index();
    auto data_after = sn.get_data();
    ASSERT_EQ(data_after.size(), data_before.size() + 2);

    std::pair<int8_t, int8_t> tail_before{data_before[2], data_before[3]};
    std::pair<int8_t, int8_t> tail_after{data_after[4], data_after[5]};
    ASSERT_EQ(tail_after.first, tail_before.first);
    ASSERT_EQ(tail_after.second, tail_before.second);
}
