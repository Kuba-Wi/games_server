#pragma once

#include <atomic>
#include <chrono>
#include <thread>

class timer {
public:
    template <typename Function>
    void start_timer(Function f, size_t interval_ms);
    void stop_timer() {
        _active = false;
        if (th.joinable()) {
            th.join();
        }
    }
    ~timer() {
        stop_timer();
    }
private:
    std::atomic<bool> _active{false};
    std::thread th;
};

template <typename Function>
void timer::start_timer(Function f, size_t interval_ms) {
    _active = false;
    if (th.joinable()) {
        th.join();
    }
    _active = true;
    th = std::thread{[=, this](){
        while (_active) {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
            if (!_active) {
                return;
            }
            f();
        }
    }};
}
