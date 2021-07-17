#include "server.h"

server::server(boost::asio::ip::tcp::socket& socket) : _socket{std::move(socket)} {
    receive_data();
}

void server::receive_data() {
    try {
        _socket.async_receive(
            boost::asio::buffer(&_data_buffer, sizeof(uint8_t)),
            [&](const boost::system::error_code& er, size_t) mutable {
                if (!er) {
                    _byte_received = _data_buffer;
                    receive_data();
                } else {
                    _socket_connected = false;
                    std::cerr << er.message() << std::endl;
                }
            });
    } catch(std::exception& e) {
        _socket_connected = false;
        std::cerr << e.what() << std::endl;
    }
}

uint8_t server::get_received_data() const {
    return _byte_received;
}
