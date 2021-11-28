#pragma once

#include <gmock/gmock.h>

#include "timer.h"

template <typename T>
class timer_mock : public timer<T> {
public:
    MOCK_METHOD(void, restart_timer, (timer_function<T>&&, size_t), (override));
    MOCK_METHOD(void, stop_timer, (), (override));
    MOCK_METHOD(bool, is_started, (), (const, override));
    MOCK_METHOD(std::optional<size_t>, get_current_interval_ms, (), (const, override));
};
