#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <thread>

class Itimer {
public:
    virtual ~Itimer() = default;
    virtual void start_timer(std::function<void()> f, size_t interval_ms) = 0;
    virtual void stop_timer() = 0;
};

class timer : public Itimer {
public:
    void start_timer(std::function<void()> f, size_t interval_ms) override {
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

    void stop_timer() override {
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
