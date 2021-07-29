#pragma once
#include <atomic>
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

    void new_food();
    bool is_food_eaten() const {
        std::scoped_lock sl(_snake_mutex, _food_mutex);
        return _snake_index.front() == _food_index; 
    }
    bool is_index_present(uint8_t row, uint8_t column) const;
    bool is_collision() const;
    void add_snake_index() { 
        std::lock_guard lg(_snake_mutex);
        _snake_index.push_back(_snake_index.back()); 
    }
    uint8_t size() const { 
        std::lock_guard lg(_snake_mutex);
        return _snake_index.size(); 
    }

    auto get_data() const {
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

    void set_current_direction(move_direction direction);
    void move();

private:
    void move_setup();
    void move_up();
    void move_down();
    void move_right();
    void move_left();
    bool is_snake_index(uint8_t row, uint8_t column) const;

    const uint8_t _height;
    const uint8_t _width;
    std::vector<std::pair<uint8_t, uint8_t>> _snake_index;
    std::pair<uint8_t, uint8_t> _food_index;
    std::atomic<move_direction> _current_direction{move_direction::up};
    std::atomic<bool> _direction_set{false};

    mutable std::mutex _snake_mutex;
    mutable std::mutex _food_mutex;
};
