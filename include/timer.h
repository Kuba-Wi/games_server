#pragma once

#include <atomic>
#include <chrono>
#include <optional>
#include <thread>

#include "test_iface.h"

template <typename T>
class timer_function {
public:
    timer_function(T& obj) : _object(obj) {}
    void operator()();
private:
    T& _object;
};

template <typename T>
class timer {
public:
    TEST_IFACE void restart_timer(timer_function<T>&& f, size_t interval_ms) {
        this->stop_timer();
        _current_interval_ms = interval_ms;
        _active = true;
        th = std::thread{[=, this]() mutable {
            while (_active) {
                std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
                if (!_active) {
                    return;
                }
                f();
            }
        }};
    }

    TEST_IFACE void stop_timer() {
        this->_active = false;
        if (th.joinable()) {
            th.join();
        }
    }

    TEST_IFACE bool is_started() const {
        return _active;
    }

    TEST_IFACE std::optional<size_t> get_current_interval_ms() const {
        if (this->is_started()) {
            return _current_interval_ms;
        }
        return std::nullopt;
    }
    
    TEST_IFACE ~timer() {
        this->stop_timer();
    }

private:
    std::atomic<bool> _active{false};
    std::atomic<size_t> _current_interval_ms{0};
    std::thread th;
};
