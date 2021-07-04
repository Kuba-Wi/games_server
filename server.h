#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>

class server {
public:
    server();
    ~server();
    void receive_data();
    template <typename T>
    void send_data(const std::vector<std::pair<T*, size_t>>& data);

    uint8_t get_received_data() const { return _data_received; }
private:
    boost::asio::io_context _io_context;
    boost::asio::ip::udp::socket _socket;
    boost::asio::ip::udp::endpoint _server_endpoint;
    boost::asio::ip::udp::endpoint _client_endpoint;

    std::thread _io_context_thread;
    uint8_t _data_received;
};

template <typename T>
void server::send_data(const std::vector<std::pair<T*, size_t>>& data) {
    boost::system::error_code er;
    std::vector<boost::asio::mutable_buffer> buf;
    for (auto& pair : data) {
        buf.emplace_back(pair.first, pair.second);
    }
    try {
        _socket.async_send_to(buf, _client_endpoint, 
            [&](const boost::system::error_code&, size_t) {});
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
