#pragma once

#include <memory>

#include <gmock/gmock.h>

class ui_iface_mock;

struct uiMockHolder {
    std::unique_ptr<ui_iface_mock> mock_ptr;
} inline ui_mock_holder;
struct ui_iface_mock {
    MOCK_METHOD(void, refresh_model, ());
    MOCK_METHOD(void, enable_sending, ());
    MOCK_METHOD(void, stop_sending, ());
    MOCK_METHOD(void, set_board_dimensions, ());
    MOCK_METHOD(void, wait_for_connection, ());
    MOCK_METHOD(void, connection_established, ());
};
