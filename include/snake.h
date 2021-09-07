#pragma once

#include <atomic>
#include <list>
#include <mutex>
#include <vector>

enum class move_direction {
    up = 0,
    down,
    right,
    left
};

class snake {
public:
    snake(uint8_t height, uint8_t width);

    void reset_snake();
    void new_food();
    bool is_food_eaten() const;
    bool is_collision() const;
    void add_snake_index();

    std::vector<int8_t> get_data() const;
    uint8_t get_board_height() const { return _height; }
    uint8_t get_board_width() const { return _width; }
    void set_current_direction(move_direction direction);
    void move();

private:
    void move_setup();
    void move_up();
    void move_down();
    void move_right();
    void move_left();
    bool is_snake_index(std::pair<uint8_t, uint8_t> index) const;

    const uint8_t _height;
    const uint8_t _width;
    std::vector<std::pair<uint8_t, uint8_t>> _snake_index;
    std::pair<uint8_t, uint8_t> _food_index;
    move_direction _current_direction;
    std::list<move_direction> _direction_queue;

    mutable std::mutex _snake_mutex;
    mutable std::mutex _food_mutex;
    std::mutex _direction_mx;
};
