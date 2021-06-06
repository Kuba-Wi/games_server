#pragma once
#include <algorithm>
#include <vector>
#include <random>

class snake {
public:
    snake() : _snake_index{{5, 5}, {6, 5}} {
        new_food();
    }

    void new_food() {
        std::random_device rd; 
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib_row(0, 9);
        std::uniform_int_distribution<> distrib_column(0, 11);
        do {
            _food_index.first = distrib_row(gen);
            _food_index.second = distrib_column(gen);
        } while(is_snake_index(_food_index.first, _food_index.second));
    }

    bool is_food_eaten() const {
        return _snake_index.front() == _food_index;
    }

    bool is_index_present(uint8_t row, uint8_t column) const {
        if (is_snake_index(row, column)) {
            return true;
        }
        return row == _food_index.first && column == _food_index.second;
    }

    bool is_collision() const {
        return std::find_if(std::next(_snake_index.begin()), _snake_index.end(), [&](auto& index){
            return index.first == _snake_index.front().first && index.second == _snake_index.front().second;
        }) != _snake_index.end();
    }

    void move_up() {
        move();
        auto pos = _snake_index.front().first;
        if (pos == 0) {
            pos = 9;
        } else {
            --pos;
        }
        _snake_index.front().first = pos;
    }

    void move_down() {
        move();
        auto pos = _snake_index.front().first;
        if (pos == 9) {
            pos = 0;
        } else {
            ++pos;
        }
        _snake_index.front().first = pos;
    }

    void move_right() {
        move();
        auto pos = _snake_index.front().second;
        if (pos == 11) {
            pos = 0;
        } else {
            ++pos;
        }
        _snake_index.front().second = pos;
    }

    void move_left() {
        move();
        auto pos = _snake_index.front().second;
        if (pos == 0) {
            pos = 11;
        } else {
            --pos;
        }
        _snake_index.front().second = pos;
    }

    void add_snake_index() {
        _snake_index.push_back(_snake_index.back());
    }

    uint8_t size() const {
        return _snake_index.size();
    }

    auto data() {
        return _snake_index;
    }

    auto snake_index_data() {
        return std::make_pair(_snake_index.data(), 
                              sizeof(decltype(_snake_index)::value_type) * _snake_index.size());
    }

    auto food_index_data() {
        return std::make_pair(&_food_index, sizeof(_food_index));
    }

private:
    void move() {
        for (uint8_t i = _snake_index.size() - 1; i > 0; --i) {
            _snake_index[i] = _snake_index[i - 1];
        }
    }

    bool is_snake_index(uint8_t row, uint8_t column) const {
        return std::find_if(_snake_index.begin(), _snake_index.end(), [=](auto& index){
            return index.first == row && index.second == column;
        }) != _snake_index.end();
    }

    std::vector<std::pair<uint8_t, uint8_t>> _snake_index;
    std::pair<uint8_t, uint8_t> _food_index;
};
