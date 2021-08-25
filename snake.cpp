#include "snake.h"
#include <algorithm>
#include <random>
#include <unordered_map>

snake::snake(uint8_t height, uint8_t width) : _height(height), _width(width) {
    this->reset_snake();
}

void snake::reset_snake() {
    std::unique_lock ul(_snake_mutex);
    _direction_queue.clear();
    _current_direction = move_direction::up;
    _snake_index.clear();
    _snake_index.emplace_back(_height / 2, _width / 2);
    _snake_index.emplace_back(_height / 2 + 1, _width / 2);
    ul.unlock();
    this->new_food();
}

void snake::new_food() {
    std::random_device rd; 
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib_row(0, _height - 1);
    std::uniform_int_distribution<> distrib_column(0, _width - 1);
    std::scoped_lock sl(_snake_mutex, _food_mutex);
    do {
        _food_index.first = distrib_row(gen);
        _food_index.second = distrib_column(gen);
    } while(is_snake_index(_food_index.first, _food_index.second));
}

bool snake::is_food_eaten() const {
    std::scoped_lock sl(_snake_mutex, _food_mutex);
    return _snake_index.front() == _food_index; 
}

bool snake::is_collision() const {
    std::lock_guard lg(_snake_mutex);
    return std::find_if(std::next(_snake_index.begin()), _snake_index.end(), [&](auto& index){
        return index == _snake_index.front();
    }) != _snake_index.end();
}

void snake::add_snake_index() { 
    std::lock_guard lg(_snake_mutex);
    _snake_index.push_back(_snake_index.back()); 
}

std::vector<int8_t> snake::get_data() const {
    std::scoped_lock sl(_snake_mutex, _food_mutex);
    std::vector<int8_t> data((_snake_index.size() + 1) * sizeof(decltype(_snake_index)::value_type));
    auto it = data.begin();
    for (auto& pair : _snake_index) {
        *(it++) = pair.first;
        *(it++) = pair.second;
    }
    *(it++) = _food_index.first;
    *(it++) = _food_index.second;

    return data;
}

void snake::move() {
    std::lock_guard lg(_snake_mutex);
    this->move_setup();
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

    if (_direction_queue.size() > 0) {
        _direction_queue.erase(_direction_queue.begin());
    }
    if (_direction_queue.size() > 0) {
        _current_direction = _direction_queue.front();
    }
}

void snake::move_up() {
    auto pos = _snake_index.front().first;
    if (pos == 0) {
        pos = _height - 1;
    } else {
        --pos;
    }
    _snake_index.front().first = pos;
}

void snake::move_down() {
    auto pos = _snake_index.front().first;
    if (pos == _height - 1) {
        pos = 0;
    } else {
        ++pos;
    }
    _snake_index.front().first = pos;
}

void snake::move_right() {
    auto pos = _snake_index.front().second;
    if (pos == _width - 1) {
        pos = 0;
    } else {
        ++pos;
    }
    _snake_index.front().second = pos;
}

void snake::move_left() {
    auto pos = _snake_index.front().second;
    if (pos == 0) {
        pos = _width - 1;
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
    using md = move_direction;
    std::unordered_map<md, md> opposite_dir{{md::up, md::down}, {md::down, md::up},
                                            {md::left, md::right}, {md::right, md::left}};

    std::lock_guard lg(_snake_mutex);
    if (_direction_queue.size() == 0) {
        if (opposite_dir[direction] != _current_direction && direction != _current_direction) {
            _direction_queue.push_back(direction);
            _current_direction = _direction_queue.front();
        }
    } else if (opposite_dir[direction] != _direction_queue.back() && direction != _direction_queue.back()) {
        _direction_queue.push_back(direction);
    }
}
