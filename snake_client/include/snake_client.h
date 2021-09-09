#pragma once

#include <atomic>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "network.h"

enum class client_signal : int8_t {
    start_sending = -1,
    initial_data = -2,
    stop_sending = -3
};

class snake_client {
public:
    snake_client(std::unique_ptr<network>&& ptr);

    void update_snake(const std::vector<int8_t>& data);
    void set_disconnected();
    void set_connected();

    bool set_server_address(const std::string& ip) { return _network_ptr->set_server_address(ip); }
    void connect_network() { _network_ptr->connect(); }

    void send_data(uint8_t data);
    bool check_index_present(uint8_t x, uint8_t y) const;
    uint8_t get_board_height() const { return _board_height; }
    uint8_t get_board_width() const { return _board_width; }

private:
    void process_received_signal(const std::vector<int8_t>& signal);
    void refresh_snake_board(const std::vector<int8_t>& data);
    void set_snake_board_size();

    mutable std::mutex _snake_board_mx;
    std::vector<std::vector<bool>> _snake_board;
    std::atomic<bool> _sending_data_enabled{false};
    std::atomic<uint8_t> _board_height{0};
    std::atomic<uint8_t> _board_width{0};

    std::unique_ptr<network> _network_ptr;
};
