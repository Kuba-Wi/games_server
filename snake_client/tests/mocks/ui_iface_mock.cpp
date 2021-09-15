#include "ui_iface.h"
#include "ui_iface_mock.h"

void refresh_model() {
    if (ui_mock_holder.mock_ptr)
        ui_mock_holder.mock_ptr->refresh_model();
}

void enable_sending() {
    if (ui_mock_holder.mock_ptr)
        ui_mock_holder.mock_ptr->enable_sending();
}

void stop_sending() {
    if (ui_mock_holder.mock_ptr)
        ui_mock_holder.mock_ptr->stop_sending();
}

void set_board_dimensions() {
    if (ui_mock_holder.mock_ptr)
        ui_mock_holder.mock_ptr->set_board_dimensions();
}

void wait_for_connection() {
    if (ui_mock_holder.mock_ptr)
        ui_mock_holder.mock_ptr->wait_for_connection();
}

void connection_established() {
    if (ui_mock_holder.mock_ptr)
        ui_mock_holder.mock_ptr->connection_established();
}
