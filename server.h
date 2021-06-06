#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <vector>

class server {
public:
    server();
    void receive_data();
    template <typename T>
    void send_data(const std::vector<std::pair<T*, size_t>>& data);
private:
    boost::asio::io_context _io_context;
    boost::asio::ip::udp::socket _socket;
    boost::asio::ip::udp::endpoint _server_endpoint;
    boost::asio::ip::udp::endpoint _client_endpoint;
};

template <typename T>
void server::send_data(const std::vector<std::pair<T*, size_t>>& data) {
    boost::system::error_code er;
    std::vector<boost::asio::mutable_buffer> buf;
    for (auto& pair : data) {
        buf.emplace_back(pair.first, pair.second);
    }
    _socket.send_to(buf, _client_endpoint, 0, er);
    if(er) {
        std::cerr << "Failed to send data:\n" << er.message() << "\n";
    }
}
