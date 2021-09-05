#pragma once

#include <atomic>
#include <chrono>
#include <thread>

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
    virtual void start_timer(timer_function<T>&& f, size_t interval_ms) {
        this->stop_timer();
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

    virtual void stop_timer() {
        _active = false;
        if (th.joinable()) {
            th.join();
        }
    }
    
    virtual ~timer() {
        this->stop_timer();
    }

private:
    std::atomic<bool> _active{false};
    std::thread th;
};