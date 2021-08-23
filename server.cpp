#include "server.h"

#include <spdlog/spdlog.h>

server::server(boost::asio::ip::tcp::socket&& socket, boost::asio::io_context& io, Iservers* servers) : 
                                                                           _socket(std::move(socket)),
                                                                           _socket_udp(io),
                                                                           _servers_observer(servers) {
    
    boost::system::error_code er;
    _socket_udp.open(boost::asio::ip::udp::v4(), er);
    if (er) {
        spdlog::error("Opening udp socket failed: {}", er.message());
    }
    auto endpoint = _socket.remote_endpoint(er);
    if (er) {
        spdlog::error("Setting udp endpoint address and port failed: {}", er.message());
    } else {
        _client_endpoint.address(endpoint.address());
        _client_endpoint.port(endpoint.port());
    }

    _socket.set_option(boost::asio::ip::tcp::no_delay(true), er);
    if (er) {
        spdlog::error("Set socket option (no delay): {}", er.message());
    }

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
    if (!_socket_connected) {
        return;
    }
    boost::asio::async_read(
        _socket,
        boost::asio::buffer(&_data_buffer, sizeof(_data_buffer)),
        [ptr = this->shared_from_this()](const boost::system::error_code& er, size_t) {
            if (!er) {
                ptr->notify_data_received();
                ptr->receive_data();
            } else {
                ptr->end_connection();
                ptr->notify_server_disconnected();
                spdlog::info("Receive: {}. Client disconnected", er.message());
            }
        });
}

void server::send_data(const send_type& data) {
    if (!_socket_connected) {
        return;
    }
    for (size_t i = 0; i < retransmition_count; ++i) {
        std::lock_guard lg(_send_udp_mx);
        _send_queue_udp.push_back(data);
        boost::asio::mutable_buffer buf(_send_queue_udp.back().data(), _send_queue_udp.back().size() * sizeof(send_type::value_type));
        auto it = std::prev(_send_queue_udp.end());

        _socket_udp.async_send_to(buf, _client_endpoint,
            [it, ptr = this->shared_from_this()](const boost::system::error_code&, size_t){
                ptr->erase_el_from_udp_queue(it);
            });
    }
}

void server::send_signal(const send_type& data) {
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

void server::execute_send_signal() {
    _send_executing = true;
    boost::asio::mutable_buffer buf(_send_queue.front().data(), _send_queue.front().size() * sizeof(send_type::value_type));
    auto it = _send_queue.begin();

    boost::asio::async_write(_socket, buf, 
        [it, ptr = this->shared_from_this()](const boost::system::error_code& er, size_t){
            if (er) {
                ptr->end_connection();
                ptr->notify_server_disconnected();
                spdlog::info("Send: {}. Client disconnected", er.message());
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
        this->execute_send_signal();
    }
}

void server::erase_el_from_queue(const send_iterator& it) {
    std::lock_guard lg(_send_mx);
    _send_queue.erase(it);
}

void server::erase_el_from_udp_queue(const send_iterator& it) {
    std::lock_guard lg(_send_udp_mx);
    _send_queue_udp.erase(it);
}
