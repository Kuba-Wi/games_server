#include "snake_client.h"
#include "ui_iface.h"
#include <iostream>

snake_client::snake_client(network& network) : _network(network) { 
    _network.attach_observer(this); 
    this->connect_network();
    this->start_receiving_data();
}

void snake_client::update_snake(const std::vector<int8_t>& data, size_t bytes_received) {
    if (data.front() < 0) {
        this->process_received_signal(data);
    } else {
        this->refresh_client_data(data, bytes_received);
        refresh_client();
    }
}

void snake_client::send_data(uint8_t data) {
    if (!_sending_data_enabled) {
        return;
    }
    _network.send_data(data);
}

void snake_client::process_received_signal(const std::vector<int8_t>& signal) {
    switch (static_cast<client_signal>(signal.front())) {
    case client_signal::start_sending:
        _sending_data_enabled = true;
        enable_sending();
        break;
    
    case client_signal::initial_data:
        _board_height = signal[1];
        _board_width = signal[2];
        set_board_dimensions();
        break;
    
    case client_signal::stop_sending:
        _sending_data_enabled = false;
        stop_sending();
        break;
    }
}

bool snake_client::check_index_present(uint8_t x, uint8_t y) const {
    std::lock_guard lg(_client_data_mx);
    return std::any_of(_client_data.begin(), _client_data.end(), [x, y](const auto& pair){
            return pair.first == x && pair.second == y;
        });
}

void snake_client::refresh_client_data(const std::vector<int8_t>& data, size_t bytes_received) {
    std::lock_guard lg(_client_data_mx);
    _client_data.resize(bytes_received / sizeof(decltype(_client_data)::value_type));
    auto it = _client_data.begin();
    for (size_t i = 0; i < bytes_received - 1; i += 2) {
        *(it++) = {data[i], data[i + 1]};
    }
}
