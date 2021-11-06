#include <gtest/gtest.h>

#include "timer.h"

struct Fake {};

template <>
void timer_function<Fake>::operator()() {}

TEST(timerTest, timerShouldBeStartedAfterStart) {
    constexpr size_t interval_ms = 1;
    Fake fake_object;
    timer<Fake> timer;

    timer.start_timer(timer_function{fake_object}, interval_ms);
    ASSERT_TRUE(timer.is_started());
}

TEST(timerTest, timerShouldBeStoppedAfterStop) {
    constexpr size_t interval_ms = 1;
    Fake fake_object;
    timer<Fake> timer;

    ASSERT_FALSE(timer.is_started());

    timer.start_timer(timer_function{fake_object}, interval_ms);
    timer.stop_timer();
    ASSERT_FALSE(timer.is_started());
}

TEST(timerTest, getCurrentIntervalShouldReturnTimeIntervalWhenTimerIsStarted) {
    constexpr size_t interval_ms = 1;
    Fake fake_object;
    timer<Fake> timer;

    timer.start_timer(timer_function{fake_object}, interval_ms);
    auto current_interval_opt = timer.get_current_interval_ms();

    ASSERT_TRUE(current_interval_opt.has_value());
    ASSERT_EQ(current_interval_opt.value(), interval_ms);
}

TEST(timerTest, getCurrentIntervalShouldNotReturnTimeIntervalWhenTimerIsStopped) {
    timer<Fake> timer;

    auto current_interval_opt = timer.get_current_interval_ms();
    ASSERT_FALSE(current_interval_opt.has_value());
}
