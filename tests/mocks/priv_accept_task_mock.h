#pragma once

#include <gmock/gmock.h>

#include "privileged_accept_task.h"
#include "privileged_connection.h"

class priv_accept_task_mock : public privileged_accept_task {
public:
    MOCK_METHOD(void, attach_observer, (privileged_connection*), (override));
    MOCK_METHOD(void, accept_connections, (), (override));
};
