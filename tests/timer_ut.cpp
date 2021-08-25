#include <gtest/gtest.h>

#include "timer.h"

TEST(timerTest, timerShouldStartAndStopWithoutCrash) {
    timer time;
    time.stop_timer();
    size_t i = 10;
    while (i-- > 0) {
        time.start_timer([](){}, i);
    }
    i = 10;
    while (i-- > 0) {
        time.start_timer([](){}, i);
        time.stop_timer();
    }
}
