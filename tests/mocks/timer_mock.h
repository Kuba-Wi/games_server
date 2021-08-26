#pragma once

#include <gmock/gmock.h>

#include "timer.h"

class timer_mock : public Itimer {
public:
    MOCK_METHOD(void, start_timer, (std::function<void()> f, size_t interval_ms), (override));
    MOCK_METHOD(void, stop_timer, (), (override));
};
