#include <gtest/gtest.h>

#include "timer.h"

struct Fake {};

template <>
void timer_function<Fake>::operator()() {}

TEST(timerTest, timerShouldStartAndStopWithoutCrash) {
    constexpr size_t execution_count = 10;
    Fake fake;
    timer<Fake> time;

    time.stop_timer();
    for (size_t i = 1; i < execution_count; ++i) {
        time.start_timer(timer_function{fake}, i);
    }

    for (size_t i = 1; i < execution_count; ++i) {
        time.start_timer(timer_function{fake}, i);
        time.stop_timer();
    }
}
