#include "snake_client.h"
#include "ui_iface.h"

snake_client::snake_client(std::unique_ptr<network>&& ptr) : _network_ptr(std::move(ptr)) {
    _network_ptr->attach_observer(this);
}

void snake_client::update_snake(const std::vector<int8_t>& data) {
    if (data.size() == 0) {
        return;
    }
    if (data.front() < 0) {
        this->process_received_signal(data);
    } else {
        this->refresh_snake_board(data);
        refresh_model();
    }
}

void snake_client::update_disconnected(const std::string& message) {
    _sending_data_enabled = false; 
    connection_error(message);
}

void snake_client::set_connected() {
    connection_established();
}

void snake_client::send_data(uint8_t data) {
    if (!_sending_data_enabled) {
        return;
    }
    _network_ptr->send_data(data);
}

void snake_client::update_connection_failed(const std::string& message) const {
    connection_error(message);
}

void snake_client::process_received_signal(const std::vector<int8_t>& signal) {
    switch (static_cast<client_signal>(signal.front())) {
    case client_signal::start_sending:
        _sending_data_enabled = true;
        enable_sending();
        break;
    
    case client_signal::initial_data:
        if (signal.size() >= 3) {
            this->resize_snake_board(signal[1], signal[2]);
            set_board_dimensions();

            refresh_model();
        }
        break;
    
    case client_signal::stop_sending:
        _sending_data_enabled = false;
        stop_sending();
        break;
    }
}

bool snake_client::check_index_present(uint8_t x, uint8_t y) const {
    std::lock_guard lg(_snake_board_mx);
    if (_snake_board.size() <= x) {
        return false;
    }
    if (_snake_board[x].size() <= y) {
        return false;
    }
    return _snake_board[x][y];
}

void snake_client::refresh_snake_board(const std::vector<int8_t>& data) {
    std::lock_guard lg(_snake_board_mx);
    this->clean_snake_board();

    for (size_t i = 0; i < data.size() - 1; i += 2) {
        if (_snake_board.size() <= uint8_t(data[i])) {
            return;
        }
        if (_snake_board[data[i]].size() <= uint8_t(data[i + 1])) {
            return;
        }
        _snake_board[data[i]][data[i + 1]] = true;
    }
}

void snake_client::resize_snake_board(uint8_t height, uint8_t width) {
    if (height == 0 || width == 0) {
        return;
    }
    _board_height = height;
    _board_width = width;

    std::lock_guard lg(_snake_board_mx);
    _snake_board.resize(_board_height);
    for (auto& row : _snake_board) {
        row.resize(_board_width);
    }
    this->clean_snake_board();
}

void snake_client::clean_snake_board() {
    for (size_t i = 0; i <_snake_board.size(); ++i) {
        for (size_t j = 0; j < _snake_board[i].size(); ++j) {
            _snake_board[i][j] = false;
        }
    }
}
