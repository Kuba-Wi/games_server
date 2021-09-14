#include "ui_iface.h"
#include "ui_iface_mock.h"

void refresh_model() {
    ui_iface_mock.refresh_model();
}

void enable_sending() {
    ui_iface_mock.enable_sending();
}

void stop_sending() {
    ui_iface_mock.stop_sending();
}

void set_board_dimensions() {
    ui_iface_mock.set_board_dimensions();
}

void wait_for_connection() {
    ui_iface_mock.wait_for_connection();
}

void connection_established() {
    ui_iface_mock.connection_established();
}
