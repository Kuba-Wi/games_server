#include "network.h"

#include <iostream>

network::network() : _socket(_io_context) {
    boost::system::error_code er_adress;
    _server_endpoint.address(boost::asio::ip::make_address("localhost", er_adress));
    _server_endpoint.port(30000);
    if (er_adress) {
        std::cerr << "Address: " << er_adress.message() << "\n";
    }

    _io_context_thread = std::thread{[&](){
        using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
        work_guard_type work_guard(_io_context.get_executor());
        _io_context.run();
    }};
}

network::~network() {
    _io_context.stop();
    _io_context_thread.join();
}

void network::connect() {
    if (!_snake_observer || _socket_connected) {
        return;
    }
    this->prepare_socket_connect();
    _socket.async_connect(_server_endpoint, [&](const boost::system::error_code& error){
        if (error) {
            this->connect();
        } else {
            std::cout << "Connected" << std::endl;
            _socket_connected = true;
            this->connect_snake();
            this->receive_data();
        }
    });
}

void network::receive_data() {
    if (!_snake_observer || !_socket_connected) {
        return;
    }
    async_read_until(_socket,
        boost::asio::dynamic_buffer(_data_received), data_delimiter,
        [&](const boost::system::error_code& er, size_t bytes_with_delimiter) {
            if (!er) {
                this->notify_snake(bytes_with_delimiter - 1);
                this->refresh_data_buffer(bytes_with_delimiter);
                this->receive_data();
            } else {
                std::cerr << "Receive: " << er.message() << std::endl;
                _socket_connected = false;
                this->disconnect_snake();
                this->connect();
            }
        });
}

void network::send_data(uint8_t data) {
    if (!_snake_observer || !_socket_connected) {
        return;
    }
    std::lock_guard lg(_send_queue_mx);
    _send_queue.push_back(data);
    boost::asio::mutable_buffer buf(&_send_queue.back(), sizeof(uint8_t));
    auto it = std::prev(_send_queue.end());

    boost::asio::async_write(_socket, buf, 
        [&, it](const boost::system::error_code& er, size_t) { 
            if (er) {
                std::cerr << "Send: " << er.message() << std::endl;
                _socket_connected = false;
                this->disconnect_snake();
                this->connect();
            }
            std::lock_guard lg(_send_queue_mx);
            _send_queue.erase(it);
        });
}

void network::refresh_data_buffer(size_t bytes_with_delimiter) {
    _data_received.erase(_data_received.begin(), _data_received.begin() + bytes_with_delimiter);
}
