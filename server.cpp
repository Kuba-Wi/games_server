#include "server.h"

server::server(boost::asio::ip::tcp::socket& socket, Iservers* servers) : _socket{std::move(socket)},
                                                                          _servers_observer{servers} {
    _send_loop_th = std::thread{[&](){
        this->send_loop();
    }};
}

server::~server() {
    this->end_connection();
    _send_data_cv.notify_all();
    _send_loop_th.join();
}

void server::receive_data() {
    boost::asio::async_read(
        _socket,
        boost::asio::buffer(&_data_buffer, sizeof(_data_buffer)),
        [ptr = this->shared_from_this()](const boost::system::error_code& er, size_t) {
            if (!er) {
                ptr->notify_servers_observer();
                ptr->receive_data();
            } else {
                ptr->end_connection();
                std::cerr << "Receive: " << er.message() << std::endl;
            }
        });
}

void server::send_data(const send_type& data) {
    if (!_socket_connected) {
        return;
    }

    std::unique_lock ul(_send_mx);
    if (_send_queue.size() >= _queue_max_size) {
        return;
    }
    _send_queue.push_back(data);
    _send_queue.back().push_back(data_delimiter);
    ul.unlock();
    _send_data_cv.notify_all();
}

void server::execute_send() {
    _send_executing = true;
    boost::asio::mutable_buffer buf(_send_queue.front().data(), _send_queue.front().size() * sizeof(send_type::value_type));
    auto it = _send_queue.begin();

    boost::asio::async_write(_socket, buf, 
        [it, ptr = this->shared_from_this()](const boost::system::error_code& er, size_t){
            if (er) {
                ptr->end_connection();
                std::cerr << "Send: " << er.message() << std::endl;
            }
            ptr->erase_el_from_queue(it);
            ptr->_send_executing = false;
            ptr->_send_data_cv.notify_all();
        });
}

void server::send_loop() {
    while (_socket_connected) {
        std::unique_lock ul(_send_mx);
        _send_data_cv.wait(ul, [&](){
            return (_send_queue.size() > 0 && !_send_executing) || !_socket_connected;
        });
        if (!_socket_connected) {
            return;
        }
        this->execute_send();
    }
}

void server::erase_el_from_queue(const send_iterator& it) {
    std::lock_guard lg(_send_mx);
    _send_queue.erase(it);
}
