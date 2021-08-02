#pragma once
#include <boost/asio.hpp>

#include <atomic>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

enum class client_signal : int8_t {
    start_sending = -1,
    initial_data = -2,
    stop_sending = -3
};

constexpr int8_t data_delimiter = std::numeric_limits<int8_t>::max();

class client_connection {
public:
    client_connection();
    ~client_connection();

    void send_data(uint8_t data);
    bool check_index_present(uint8_t x, uint8_t y) const;
    uint8_t get_board_height() const { return _board_height; }
    uint8_t get_board_width() const { return _board_width; }

private:
    void process_received_signal(const std::vector<int8_t>& signal);
    void refresh_client_data(size_t bytes_received);
    void receive_data();
    void refresh_data_buffer(size_t bytes_with_delimiter);
    
    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::endpoint _server_endpoint;
    boost::asio::ip::tcp::socket _socket;

    std::thread _io_context_thread;
    mutable std::mutex _client_data_mx;

    std::mutex _send_queue_mx;
    std::list<int8_t> _send_queue;

    std::vector<int8_t> _data_received;
    std::vector<std::pair<uint8_t, uint8_t>> _client_data;
    std::atomic<bool> _sending_data_enabled{false};
    std::atomic<uint8_t> _board_height{0};
    std::atomic<uint8_t> _board_width{0};
};
