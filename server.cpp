#include "server.h"

void server::receive_data() {
    try {
        boost::asio::async_read(
            _socket,
            boost::asio::buffer(&_data_buffer, sizeof(uint8_t)),
            [ptr = this->shared_from_this()](const boost::system::error_code& er, size_t) {
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

    this->execute_send(buf, it);
}

void server::send_client_signal(client_signal signal) {
    if (!_socket_connected) {
        return;
    }
    std::unique_lock ul(_signal_mutex);
    _signal_queue.push_back(static_cast<int8_t>(signal));
    boost::asio::mutable_buffer buf(&_signal_queue.back(), sizeof(_signal_queue.back()) * sizeof(int8_t));
    auto it = std::prev(_signal_queue.end());
    ul.unlock();

    this->execute_send(buf, it);
}

uint8_t server::get_received_data() const {
    return _byte_received;
}

void server::update_byte_received() {
    _byte_received = _data_buffer;
}
