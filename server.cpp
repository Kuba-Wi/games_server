#include "server.h"

void server::receive_data() {
    try {
        _socket.async_receive(
            boost::asio::buffer(&_data_buffer, sizeof(uint8_t)),
            [ptr = this->shared_from_this()](const boost::system::error_code& er, size_t) mutable {
                if (!er) {
                    ptr->update_byte_received();
                    ptr->receive_data();
                } else {
                    ptr->end_connection();
                    std::cerr << er.message() << std::endl;
                }
            });
    } catch(std::exception& e) {
        _socket_connected = false;
        std::cerr << e.what() << std::endl;
    }
}

void server::send_data(const send_type& data) {
    if (!_socket_connected) {
        return;
    }
    std::unique_lock ul(_send_mutex);
    _send_queue.push_back(data);
    boost::asio::mutable_buffer buf(_send_queue.back().data(), _send_queue.back().size() * sizeof(send_type::value_type));
    auto it = std::prev(_send_queue.end());
    ul.unlock();
    send_handler handler{this->shared_from_this(), it};
    
    try {
        boost::asio::async_write(_socket, buf, handler);
    } catch (std::exception& e) {
        _socket_connected = false;
        std::cerr << e.what() << std::endl;
    }
}

uint8_t server::get_received_data() const {
    return _byte_received;
}

void server::erase_el_from_queue(const send_iterator& it) {
    std::lock_guard lg(_send_mutex);
    _send_queue.erase(it);
}

void server::update_byte_received() {
    _byte_received = _data_buffer;
}
