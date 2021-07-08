#include "snake.h"
#include <algorithm>
#include <random>
#include <unordered_map>

void snake::new_food() {
    std::random_device rd; 
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib_row(0, 9);
    std::uniform_int_distribution<> distrib_column(0, 11);
    std::scoped_lock sl(_snake_mutex, _food_mutex);
    do {
        _food_index.first = distrib_row(gen);
        _food_index.second = distrib_column(gen);
    } while(is_snake_index(_food_index.first, _food_index.second));
}

bool snake::is_index_present(uint8_t row, uint8_t column) const {
    std::scoped_lock sl(_snake_mutex, _food_mutex);
    if (is_snake_index(row, column)) {
        return true;
    }
    return row == _food_index.first && column == _food_index.second;
}

bool snake::is_collision() const {
    std::lock_guard lg(_snake_mutex);
    return std::find_if(std::next(_snake_index.begin()), _snake_index.end(), [&](auto& index){
        return index.first == _snake_index.front().first && index.second == _snake_index.front().second;
    }) != _snake_index.end();
}

void snake::move() {
    std::lock_guard lg(_snake_mutex);
    switch (_current_direction) {
    case move_direction::up:
        move_up();
        break;
    
    case move_direction::down:
        move_down();
        break;
    
    case move_direction::right:
        move_right();
        break;
    
    case move_direction::left:
        move_left();
        break;
    }
    _direction_set = false;
}

void snake::move_up() {
    move_setup();
    auto pos = _snake_index.front().first;
    if (pos == 0) {
        pos = 9;
    } else {
        --pos;
    }
    _snake_index.front().first = pos;
}

void snake::move_down() {
    move_setup();
    auto pos = _snake_index.front().first;
    if (pos == 9) {
        pos = 0;
    } else {
        ++pos;
    }
    _snake_index.front().first = pos;
}

void snake::move_right() {
    move_setup();
    auto pos = _snake_index.front().second;
    if (pos == 11) {
        pos = 0;
    } else {
        ++pos;
    }
    _snake_index.front().second = pos;
}

void snake::move_left() {
    move_setup();
    auto pos = _snake_index.front().second;
    if (pos == 0) {
        pos = 11;
    } else {
        --pos;
    }
    _snake_index.front().second = pos;
}

void snake::move_setup() {
    for (uint8_t i = _snake_index.size() - 1; i > 0; --i) {
        _snake_index[i] = _snake_index[i - 1];
    }
}

bool snake::is_snake_index(uint8_t row, uint8_t column) const {
    return std::find_if(_snake_index.begin(), _snake_index.end(), [=](auto& index){
        return index.first == row && index.second == column;
    }) != _snake_index.end();
}

void snake::set_current_direction(move_direction direction) {
    if (_direction_set) {
        return;
    }
    using md = move_direction;
    std::unordered_map<md, md> opposite_dir{{md::up, md::down}, {md::down, md::up},
                                            {md::left, md::right}, {md::right, md::left}};
    if (opposite_dir[direction] != _current_direction && _current_direction != direction) {
        _current_direction = direction;
        _direction_set = true;
    }
}
