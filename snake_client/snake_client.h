#pragma once

#include <atomic>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

#include "network.h"

enum class client_signal : int8_t {
    start_sending = -1,
    initial_data = -2,
    stop_sending = -3
};

class snake_client : public Isnake_client {
public:
    snake_client(network& network);
    void update_snake(const std::vector<int8_t>& data, size_t bytes_received) override;
    void set_disconnected() override;
    void set_connected() override;

    void send_data(uint8_t data);
    bool check_index_present(uint8_t x, uint8_t y) const;
    uint8_t get_board_height() const { return _board_height; }
    uint8_t get_board_width() const { return _board_width; }

private:
    void connect_network() { _network.connect(); }
    void process_received_signal(const std::vector<int8_t>& signal);
    void refresh_client_data(const std::vector<int8_t>& data, size_t bytes_received);

    mutable std::mutex _client_data_mx;
    std::vector<std::pair<uint8_t, uint8_t>> _client_data;
    std::atomic<bool> _sending_data_enabled{false};
    std::atomic<uint8_t> _board_height{0};
    std::atomic<uint8_t> _board_width{0};

    network& _network;
};
