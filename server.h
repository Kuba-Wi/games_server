#pragma once

#include <boost/asio.hpp>

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

class server {
public:
    server();
    ~server();
    void receive_data();
    template <typename T>
    void send_data(const std::pair<std::vector<T>, size_t>& data);

    uint8_t get_received_data() const;
private:
    boost::asio::io_context _io_context;
    boost::asio::ip::udp::socket _socket;
    boost::asio::ip::udp::endpoint _server_endpoint;
    boost::asio::ip::udp::endpoint _client_endpoint;

    std::thread _io_context_thread;
    uint8_t _data_buffer;
    std::atomic<uint8_t> _byte_received{0};
    std::vector<std::pair<uint8_t, uint8_t>> _data_to_send;

    std::mutex _send_mutex;
};

template <typename T>
void server::send_data(const std::pair<std::vector<T>, size_t>& data) {
    std::unique_lock ul(_send_mutex);
    boost::system::error_code er;
    _data_to_send = data.first;
    boost::asio::mutable_buffer buf(_data_to_send.data(), _data_to_send.size() * sizeof(T));
    try {
        _socket.async_send_to(buf, _client_endpoint, 
            [&, ul = std::move(ul)](const boost::system::error_code&, size_t) mutable { ul.unlock(); });
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
