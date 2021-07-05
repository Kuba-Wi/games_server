#include "server.h"
#include <iostream>
#include <vector>

server::server() : _socket(_io_context) {
    boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::udp::v4(), 30000);
    boost::system::error_code er;
    _socket.open(endpoint.protocol(), er);
    if (er) {
        std::cerr << er.message() << "\n";
    } else {
        _socket.bind(endpoint, er);
        if (er) {
            std::cerr << er.message() << "\n";
        }
    }

    receive_data();
    _io_context_thread = std::thread{[&](){
        _io_context.run();
    }};
}

server::~server() {
    _io_context.stop();
    _io_context_thread.join();
}

void server::receive_data() {
    try {
        _socket.async_receive_from(
            boost::asio::buffer(&_data_buffer, sizeof(uint8_t)),
            _client_endpoint,
            [&](const boost::system::error_code& er, size_t) mutable {
                if (!er) {
                    _byte_received = _data_buffer;
                    receive_data();
                } else {
                    std::cerr << er.message() << "\n";
                }
            });
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

uint8_t server::get_received_data() const {
    return _byte_received;
}
