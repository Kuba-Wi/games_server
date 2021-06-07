#pragma once
#include <cstdint>
#include <vector>

class snake {
public:
    snake() : _snake_index{{5, 5}, {6, 5}} { new_food(); }

    void new_food();
    bool is_food_eaten() const { return _snake_index.front() == _food_index; }
    bool is_index_present(uint8_t row, uint8_t column) const;
    bool is_collision() const;

    void move_up();
    void move_down();
    void move_right();
    void move_left();

    void add_snake_index() { _snake_index.push_back(_snake_index.back()); }
    uint8_t size() const { return _snake_index.size(); }

    auto snake_index_data() {
        return std::make_pair(_snake_index.data(), 
                          sizeof(decltype(_snake_index)::value_type) * _snake_index.size());
    }
    auto food_index_data() { return std::make_pair(&_food_index, sizeof(_food_index)); }

private:
    void move();
    bool is_snake_index(uint8_t row, uint8_t column) const;

    std::vector<std::pair<uint8_t, uint8_t>> _snake_index;
    std::pair<uint8_t, uint8_t> _food_index;
};
