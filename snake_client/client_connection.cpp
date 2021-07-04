#include "client_connection.h"
#include <iostream>

client_connection::client_connection() : _socket(_io_context) {
    boost::system::error_code er_adress;
    boost::system::error_code er_open;
    _server_endpoint.address(boost::asio::ip::make_address("localhost", er_adress));
    _server_endpoint.port(30000);
    _socket.open(boost::asio::ip::udp::v4(), er_open);
    if (er_adress) {
        std::cerr << er_adress.message() << "\n";
    }
    if (er_open) {
        std::cerr << er_open.message() << "\n";
    }
    _data_received.resize(200);
    send_data(0);
    receive_data();

    _io_context_thread = std::thread{[&](){
        _io_context.run();
    }};
}

client_connection::~client_connection() {
    _io_context.stop();
    _io_context_thread.join();
}

void client_connection::send_data(uint8_t data) {
    try {
        _socket.async_send_to(
            boost::asio::buffer(&data, sizeof(data)), 
            _server_endpoint,
            [&](const boost::system::error_code&, size_t) {});
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void client_connection::receive_data() {
    try {
        std::lock_guard<std::mutex> lg(_data_mutex);
        _socket.async_receive_from(
            boost::asio::buffer(_data_received), 
            _server_endpoint,
            [&](const boost::system::error_code& er, size_t bytes_received){
                if (!er) {
                    _bytes_received = bytes_received;
                    receive_data();
                } else {
                    std::cerr << er.message();
                }
            });
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

bool client_connection::check_index_present(uint8_t x, uint8_t y) const {
    std::lock_guard<std::mutex> lg(_data_mutex);
    return std::any_of(_data_received.begin(), _data_received.begin() + _bytes_received / 2, [x, y](auto& pair){
            return pair.first == x && pair.second == y;
        });
}
