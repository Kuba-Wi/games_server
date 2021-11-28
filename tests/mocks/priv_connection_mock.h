#pragma once

#include <gmock/gmock.h>

#include "privileged_connection.h"

class priv_connection_mock : public privileged_connection {
public:
    priv_connection_mock(std::unique_ptr<privileged_accept_task>&& acc_task) : privileged_connection(std::move(acc_task)) {}
    MOCK_METHOD(void, attach_observer, (game_server*), (override));
    MOCK_METHOD(void, send_data, (const std::vector<int8_t>&), (override));
    MOCK_METHOD(void, send_clients_count, (size_t), (override));
};
