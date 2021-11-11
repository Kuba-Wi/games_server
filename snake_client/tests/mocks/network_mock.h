#pragma once

#include <gmock/gmock.h>

#include "network.h"

class network_mock : public network {
public:
    MOCK_METHOD(void, attach_observer, (snake_client*), (override));
    MOCK_METHOD(void, connect, (), (override));
    MOCK_METHOD(void, send_data, (uint8_t data), (override));
    MOCK_METHOD(void, update_data_received, (const std::vector<int8_t>&), (override));
    MOCK_METHOD(void, update_disconnected, (const std::string&), (override));
};
