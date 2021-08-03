#include "network.h"

#include <iostream>

network::network() : _socket(_io_context) {}

network::~network() {
    _io_context.stop();
    _io_context_thread.join();
}

void network::connect() {
    if (!_snake_observer) {
        return;
    }
    boost::system::error_code er_adress;
    boost::system::error_code er_connect;
    _server_endpoint.address(boost::asio::ip::make_address("localhost", er_adress));
    _server_endpoint.port(30000);
    if (er_adress) {
        std::cerr << "Address: " << er_adress.message() << "\n";
    }
    _socket.connect(_server_endpoint, er_connect);
    if (er_connect) {
        std::cerr << "Connect: " << er_connect.message() << "\n";
    }

    _io_context_thread = std::thread{[&](){
        using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
        work_guard_type work_guard(_io_context.get_executor());
        _io_context.run();
    }};
}

void network::receive_data() {
    if (!_snake_observer) {
        return;
    }
    try {
        async_read_until(_socket,
            boost::asio::dynamic_buffer(_data_received), data_delimiter,
            [&](const boost::system::error_code& er, size_t bytes_with_delimiter) {
                if (!er) {
                    this->notify_snake(bytes_with_delimiter - 1);
                    this->refresh_data_buffer(bytes_with_delimiter);
                    this->receive_data();
                } else {
                    std::cerr << "Receive: " << er.message() << std::endl;
                }
            });
    } catch (std::exception& e) {
        std::cerr << "Receive exception: " << e.what() << std::endl;
    }
}

void network::send_data(uint8_t data) {
    try {
        std::lock_guard lg(_send_queue_mx);
        _send_queue.push_back(data);
        boost::asio::mutable_buffer buf(&_send_queue.back(), sizeof(uint8_t));
        auto it = std::prev(_send_queue.end());

        boost::asio::async_write(_socket, buf, 
            [&, it](const boost::system::error_code& er, size_t) { 
                if (er) {
                    std::cerr << "Send: " << er.message() << std::endl;
                }
                std::lock_guard lg(_send_queue_mx);
                _send_queue.erase(it);
            });
    } catch (std::exception& e) {
        std::cerr << "Send exception: " << e.what() << std::endl;
    }
}

void network::refresh_data_buffer(size_t bytes_with_delimiter) {
    _data_received.erase(_data_received.begin(), _data_received.begin() + bytes_with_delimiter);
}
