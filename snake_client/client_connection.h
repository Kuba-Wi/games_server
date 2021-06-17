#pragma once
#include <boost/asio.hpp>

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

class client_connection {
public:
    client_connection();
    ~client_connection();

    void send_data(uint8_t data);
    bool check_index_present(uint8_t x, uint8_t y) const;
private:
    void receive_data();
    
    boost::asio::io_context _io_context;
    boost::asio::ip::udp::endpoint _client_endpoint;
    boost::asio::ip::udp::endpoint _server_endpoint;
    boost::asio::ip::udp::socket _socket;
    std::thread _io_context_thread;
    mutable std::mutex _data_mutex;
    std::atomic<size_t> _bytes_received{0};

    std::vector<std::pair<uint8_t, uint8_t>> _data_received;
};
