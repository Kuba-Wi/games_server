#pragma once

#include <string>

void refresh_model();
void enable_sending();
void stop_sending();
void set_board_dimensions();
void wait_for_connection();
void connection_established();
void connection_error(const std::string& message);
