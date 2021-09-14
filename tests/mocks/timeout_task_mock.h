#pragma once

#include <gmock/gmock.h>

#include "timeout_task.h"

class timeout_task_mock : public timeout_task {
public:
    MOCK_METHOD(void, attach_observer, (servers*), (override));
    MOCK_METHOD(void, reset_deadline, (), (override));
};
