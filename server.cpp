#include "server.h"

server::server(boost::asio::ip::tcp::socket& socket) : _socket{std::move(socket)} {
    _execute_send_th = std::thread{[&](){
        this->send_loop();
    }};
}

server::~server() {
    this->end_connection();
    _send_data_cv.notify_all();
    _execute_send_th.join();
}

void server::receive_data() {
    try {
        boost::asio::async_read(
            _socket,
            boost::asio::buffer(&_data_buffer, sizeof(_data_buffer)),
            [ptr = this->shared_from_this()](const boost::system::error_code& er, size_t) {
                if (!er) {
                    ptr->update_byte_received();
                    ptr->receive_data();
                } else {
                    ptr->end_connection();
                    std::cerr << "Receive: " << er.message() << std::endl;
                }
            });
    } catch(std::exception& e) {
        this->end_connection();
        std::cerr << "Receive exception: " << e.what() << std::endl;
    }
}

void server::send_data(const send_type& data) {
    if (!_socket_connected) {
        return;
    }

    auto data_to_send = data;
    data_to_send.push_back(data_delimiter);
    
    std::unique_lock ul(_send_mx);
    _send_queue.push_back(data_to_send);
    ul.unlock();
    _send_data_cv.notify_all();
}

std::optional<uint8_t> server::get_received_data() {
    std::lock_guard lg(_byte_received_mx);
    auto byte = _byte_received;
    _byte_received.reset();
    return byte;
}

void server::update_byte_received() {
    std::lock_guard lg(_byte_received_mx);
    _byte_received = _data_buffer;
}

void server::execute_send() {
    _sending_blocked = true;
    boost::asio::mutable_buffer buf(_send_queue.front().data(), _send_queue.front().size() * sizeof(send_type::value_type));
    auto it = _send_queue.begin();
    try {
        boost::asio::async_write(_socket, buf, 
            [it, ptr = this->shared_from_this()](const boost::system::error_code& er, size_t){
                if (er) {
                    ptr->end_connection();
                    std::cerr << "Send: " << er.message() << std::endl;
                }
                ptr->erase_el_from_queue(it);
                ptr->_sending_blocked = false;
                ptr->_send_data_cv.notify_all();
            });
    } catch (std::exception& e) {
        this->end_connection();
        std::cerr << "Send exception: " << e.what() << std::endl;
    }
}

void server::send_loop() {
    while (_socket_connected) {
        std::unique_lock ul(_send_mx);
        _send_data_cv.wait(ul, [&](){
            return (_send_queue.size() > 0 && !_sending_blocked) || !_socket_connected;
        });
        if (!_socket_connected) {
            return;
        }
        this->execute_send();
        ul.unlock();
    }
}

void server::erase_el_from_queue(const send_iterator& it) {
    std::lock_guard lg(_send_mx);
    _send_queue.erase(it);
}
