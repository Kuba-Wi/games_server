#pragma once

#include <gmock/gmock.h>

class ui_iface_m {
public:
    MOCK_METHOD(void, refresh_model, ());
    MOCK_METHOD(void, enable_sending, ());
    MOCK_METHOD(void, stop_sending, ());
    MOCK_METHOD(void, set_board_dimensions, ());
    MOCK_METHOD(void, wait_for_connection, ());
    MOCK_METHOD(void, connection_established, ());
} inline ui_iface_mock;
