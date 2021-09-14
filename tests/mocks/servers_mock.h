#pragma once

#include <gmock/gmock.h>

#include "servers.h"

class servers_mock : public servers {
public:
    servers_mock(std::unique_ptr<accept_task>&& accept, std::unique_ptr<timeout_task>&& timeout) :
                 servers(std::move(accept), std::move(timeout)) {}

    MOCK_METHOD(void, attach_observer, (game_server*), (override));
    MOCK_METHOD(void, set_initial_data, (const send_type&), (override));
    MOCK_METHOD(void, send_data, (const send_type&), (override));
    MOCK_METHOD(void, change_receiving_server, (), (override));
};
