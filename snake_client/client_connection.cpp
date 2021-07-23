#include "client_connection.h"
#include <iostream>

client_connection::client_connection() : _socket(_io_context) {
    boost::system::error_code er_adress;
    boost::system::error_code er_connect;
    _server_endpoint.address(boost::asio::ip::make_address("localhost", er_adress));
    _server_endpoint.port(30000);
    if (er_adress) {
        std::cerr << er_adress.message() << "\n";
    }
    _socket.connect(_server_endpoint, er_connect);
    if (er_connect) {
        std::cerr << er_connect.message() << "\n";
    }
    _data_received.resize(200);
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
        std::unique_lock ul(_send_mutex);
        _data_to_send = data;
        boost::asio::async_write(
            _socket,
            boost::asio::buffer(&_data_to_send, sizeof(_data_to_send)), 
            [&, ul = std::move(ul)](const boost::system::error_code&, size_t) mutable { ul.unlock(); });
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void client_connection::receive_data() {
    try {
        std::unique_lock ul(_data_mutex);
        _keep_rec_cv.wait(ul, [&](){ return _keep_receiving.load(); });
        _socket.async_receive(
            boost::asio::buffer(_data_received), 
            [&, ul = std::move(ul)](const boost::system::error_code& er, size_t bytes_received) mutable {
                if (!er) {
                    _bytes_received = bytes_received;
                    ul.unlock();
                    receive_data();
                } else {
                    std::cerr << er.message() << std::endl;
                }
            });
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

bool client_connection::check_index_present(uint8_t x, uint8_t y) const {
    std::lock_guard lg(_client_data_mutex);
    return std::any_of(_client_data.begin(), _client_data.end(), [x, y](const auto& pair){
            return pair.first == x && pair.second == y;
        });
}

void client_connection::refresh_client() {
    _keep_receiving = false;
    {
        std::scoped_lock sl(_data_mutex, _client_data_mutex);
        _client_data.resize(_bytes_received / sizeof(decltype(_client_data)::value_type));
        std::copy(_data_received.begin(), 
                  _data_received.begin() + _bytes_received / sizeof(decltype(_client_data)::value_type), 
                  _client_data.begin());
        _keep_receiving = true;
    }
    _keep_rec_cv.notify_all();
}
