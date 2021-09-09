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

void snake_client::set_disconnected() {
    _sending_data_enabled = false; 
    wait_for_connection();
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

void snake_client::process_received_signal(const std::vector<int8_t>& signal) {
    switch (static_cast<client_signal>(signal.front())) {
    case client_signal::start_sending:
        _sending_data_enabled = true;
        enable_sending();
        break;
    
    case client_signal::initial_data:
        if (signal.size() >= 3) {
            _board_height = signal[1];
            _board_width = signal[2];
            this->set_snake_board_size();
            set_board_dimensions();
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
    for (size_t i = 0; i <_snake_board.size(); ++i) {
        for (size_t j = 0; j < _snake_board[i].size(); ++j) {
            _snake_board[i][j] = false;
        }
    }

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

void snake_client::set_snake_board_size() {
    if (_board_width == 0 || _board_height == 0) {
        return;
    }
    std::lock_guard lg(_snake_board_mx);
    _snake_board.resize(_board_height);
    for (auto& row : _snake_board) {
        row.resize(_board_width);
    }
}
