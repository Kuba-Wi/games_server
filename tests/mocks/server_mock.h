#pragma once

#include <gmock/gmock.h>

#include "server.h"
#include "servers.h"

constexpr uint8_t data_received = 1;

class server_mock : public server {
public:
    server_mock(boost::asio::ip::tcp::socket&& socket, servers* servers) : server(std::move(socket), servers) {}
    MOCK_METHOD(void, send_data, (const send_type&), (override));
    MOCK_METHOD(void, receive_data, (), (override));
};
