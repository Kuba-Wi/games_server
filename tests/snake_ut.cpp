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
    std::pair snake_head{data[0], data[1]};
    std::pair snake_tail{data[2], data[3]};
    std::pair food_index{data[4], data[5]};

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
    constexpr size_t before_size = 6;
    constexpr size_t after_size = before_size + 2;

    auto data_before = sn.get_data();
    ASSERT_EQ(data_before.size(), before_size);

    sn.add_snake_index();
    auto data_after = sn.get_data();
    ASSERT_EQ(data_after.size(), after_size);

    std::pair tail_before{data_before[2], data_before[3]};
    std::pair tail_after{data_after[4], data_after[5]};
    ASSERT_EQ(tail_after, tail_before);
}

TEST_F(snakeTest, moveFunctionShouldMoveSnake) {
    constexpr size_t data_size = 6;

    auto data_before = sn.get_data();
    ASSERT_EQ(data_before.size(), data_size);
    std::pair head_before{data_before[0], data_before[1]};
    std::pair tail_before{data_before[2], data_before[3]};

    sn.set_current_direction(move_direction::up);
    sn.move();
    auto data_after = sn.get_data();
    ASSERT_EQ(data_after.size(), data_size);
    std::pair head_after{data_after[0], data_after[1]};
    std::pair tail_after{data_after[2], data_after[3]};

    ASSERT_EQ(head_after.first, head_before.first - 1);
    ASSERT_EQ(head_after.second, head_before.second);
    ASSERT_EQ(tail_after.first, tail_before.first - 1);
    ASSERT_EQ(tail_after.second, tail_before.second);
}

TEST_F(snakeTest, isCollisionShouldCheckIfSnakeHeadIsEqualSomeBodyIndex) {
    ASSERT_FALSE(sn.is_collision());
    
    sn.add_snake_index();
    sn.move();
    sn.add_snake_index();
    sn.move();
    sn.add_snake_index();
    sn.set_current_direction(move_direction::right);
    sn.move();
    sn.set_current_direction(move_direction::down);
    sn.move();
    sn.set_current_direction(move_direction::left);
    sn.move();

    ASSERT_TRUE(sn.is_collision());
}

TEST_F(snakeTest, isFoodEatenAndNewFoodShouldCheckAndChangeFoodIndex) {
    ASSERT_FALSE(sn.is_food_eaten());

    constexpr size_t data_size = 6;
    const auto data = sn.get_data();
    ASSERT_EQ(data.size(), data_size);

    std::pair head{data[0], data[1]};
    std::pair food{data[4], data[5]};
    int horizontal_dist = food.second - head.second;
    int veritical_dist = head.first - food.first;

    if (veritical_dist < 0) {
        veritical_dist += sn.get_board_height();
    }
    while (veritical_dist-- > 0) {
        sn.move();
    }

    sn.set_current_direction(move_direction::right);
    if (horizontal_dist < 0) {
        horizontal_dist += sn.get_board_width();
    }
    while (horizontal_dist-- > 0) {
        sn.move();
    }
    ASSERT_TRUE(sn.is_food_eaten());

    sn.new_food();
    const auto new_data = sn.get_data();
    ASSERT_EQ(new_data.size(), data_size);
    head = std::pair{new_data[0], new_data[1]};
    std::pair tail{new_data[2], new_data[3]};
    food = std::pair{new_data[4], new_data[5]};

    ASSERT_NE(head, food);
    ASSERT_NE(tail, food);
}
