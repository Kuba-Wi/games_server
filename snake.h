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
    snake() : _snake_index{{5, 5}, {6, 5}} { new_food(); }

    void new_food();
    bool is_food_eaten() const {
        std::scoped_lock sl(_snake_mutex, _food_mutex);
        return _snake_index.front() == _food_index; 
    }
    bool is_index_present(uint8_t row, uint8_t column) const;
    bool is_collision() const;
    void add_snake_index() { 
        std::lock_guard lg_snake(_snake_mutex);
        _snake_index.push_back(_snake_index.back()); 
    }
    uint8_t size() const { 
        std::lock_guard lg_snake(_snake_mutex);
        return _snake_index.size(); 
    }

    auto snake_index_data() {
        std::lock_guard lg_snake(_snake_mutex);
        return std::make_pair(_snake_index.data(), 
                          sizeof(decltype(_snake_index)::value_type) * _snake_index.size());
    }
    auto food_index_data() { 
        std::lock_guard lg_food(_food_mutex);
        return std::make_pair(&_food_index, sizeof(_food_index)); 
    }
    void set_current_direction(move_direction direction) { _current_direction = direction; }
    void move();

private:
    void move_setup();
    void move_up();
    void move_down();
    void move_right();
    void move_left();
    bool is_snake_index(uint8_t row, uint8_t column) const;

    std::vector<std::pair<uint8_t, uint8_t>> _snake_index;
    std::pair<uint8_t, uint8_t> _food_index;
    std::atomic<move_direction> _current_direction{move_direction::up};

    mutable std::mutex _snake_mutex;
    mutable std::mutex _food_mutex;
};
