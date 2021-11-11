#include "qt_ui_iface.h"
#include "ui_iface.h"

void refresh_model() {
    qt_ui_if.refresh_model();
}

void enable_sending() {
    qt_ui_if.enable_sending();
}

void stop_sending() {
    qt_ui_if.stop_sending();
}

void set_board_dimensions() {
    qt_ui_if.set_board_dimensions();
}

void wait_for_connection() {
    qt_ui_if.wait_for_connection();
}

void connection_established() {
    qt_ui_if.connection_established();
}

void connection_error(const std::string& message) {
    qt_ui_if.connection_error(message);
}
