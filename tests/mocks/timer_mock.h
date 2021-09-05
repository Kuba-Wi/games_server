#pragma once

#include <gmock/gmock.h>

#include "timer.h"

template <typename T>
class timer_mock : public timer<T> {
public:
    MOCK_METHOD(void, start_timer, (timer_function<T>&& f, size_t interval_ms), (override));
    MOCK_METHOD(void, stop_timer, (), (override));
};
