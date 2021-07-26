#include "client_connection.h"
#include "ui_iface.h"
#include <iostream>

client_connection::client_connection() : _socket(_io_context) {
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
    _data_received.resize(200);
    receive_data();

    _io_context_thread = std::thread{[&](){
        using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
        work_guard_type work_guard(_io_context.get_executor());
        _io_context.run();
    }};
}

client_connection::~client_connection() {
    _io_context.stop();
    _io_context_thread.join();
}

void client_connection::send_data(int8_t data) {
    if (!_sending_data_enabled) {
        return;
    }
    try {
        std::unique_lock ul(_send_queue_mx);
        _send_queue.push_back(data);
        boost::asio::mutable_buffer buf(&_send_queue.back(), sizeof(int8_t));
        auto it = std::prev(_send_queue.end());
        ul.unlock();
        boost::asio::async_write(
            _socket,
            buf, 
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

void client_connection::receive_data() {
    try {
        _socket.async_receive(
            boost::asio::buffer(_data_received), 
            [&](const boost::system::error_code& er, size_t bytes_received) {
                if (!er) {
                    if (_data_received.front().first < 0) {
                        this->process_received_signal(_data_received.front().first);
                        this->send_data(_data_received.front().first);
                    } else {
                        this->refresh_client_data(bytes_received);
                        refresh_client();
                    }
                    receive_data();
                } else {
                    std::cerr << "Receive: " << er.message() << std::endl;
                }
            });
    } catch (std::exception& e) {
        std::cerr << "Receive exception: " << e.what() << std::endl;
    }
}

void client_connection::process_received_signal(int8_t signal) {
    if (static_cast<client_signal>(signal) == client_signal::start_sending) {
        _sending_data_enabled = true;
        enable_sending();
    }
}

bool client_connection::check_index_present(uint8_t x, uint8_t y) const {
    std::lock_guard lg(_client_data_mx);
    return std::any_of(_client_data.begin(), _client_data.end(), [x, y](const auto& pair){
            return pair.first == x && pair.second == y;
        });
}

void client_connection::refresh_client_data(size_t bytes_received) {
    std::lock_guard lg(_client_data_mx);
    if (_data_received.front().first < 0) {
        return;
    }
    _client_data.resize(bytes_received / sizeof(decltype(_client_data)::value_type));
    std::copy(_data_received.begin(), 
              _data_received.begin() + bytes_received / sizeof(decltype(_client_data)::value_type), 
              _client_data.begin()); 
}
