#pragma once

#include <gmock/gmock.h>

#include <boost/asio.hpp>

#include "accept_task.h"
#include "server_mock.h"
#include "servers.h"

class accept_task_mock : public accept_task {
public:
    MOCK_METHOD(void, attach_observer, (servers* observer), (override));
    MOCK_METHOD(void, accept_connections, (), (override));
};
