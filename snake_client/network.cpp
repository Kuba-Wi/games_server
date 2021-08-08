#include "network.h"

#include <iostream>

network::network() : _socket(_io_context) {
    _server_endpoint.port(port_number);

    _io_context_thread = std::thread{[&](){
        using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
        work_guard_type work_guard(_io_context.get_executor());
        _io_context.run();
    }};

    _send_loop_th = std::thread{[&](){
        this->send_loop();
    }};
}

network::~network() {
    _stop_send_loop = true;
    _send_data_cv.notify_all();
    _send_loop_th.join();

    _io_context.stop();
    _io_context_thread.join();
}

bool network::set_server_address(const std::string& ip) {
    boost::system::error_code er_adress;
    auto address = boost::asio::ip::make_address(ip, er_adress);
    if (er_adress) {
        return false;
    }
    _server_endpoint.address(address);
    _address_set = true;
    return true;
}

void network::connect() {
    if (!_snake_observer || _socket_connected || !_address_set) {
        return;
    }
    this->prepare_socket_connect();
    _socket.async_connect(_server_endpoint, [&](const boost::system::error_code& error){
        if (error) {
            this->connect();
        } else {
            _socket_connected = true;
            this->notify_connected();
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
                this->notify_update(bytes_with_delimiter - 1);
                this->refresh_data_buffer(bytes_with_delimiter);
                this->receive_data();
            } else {
                std::cerr << "Receive: " << er.message() << std::endl;
                _socket_connected = false;
                this->notify_disconnected();
                this->connect();
            }
        });
}

void network::send_data(uint8_t data) {
    if (!_snake_observer || !_socket_connected) {
        return;
    }

    std::unique_lock ul(_send_queue_mx);
    _send_queue.push_back(data);
    ul.unlock();
    _send_data_cv.notify_all();
}

void network::refresh_data_buffer(size_t bytes_with_delimiter) {
    _data_received.erase(_data_received.begin(), _data_received.begin() + bytes_with_delimiter);
}

void network::send_loop() {
    while (!_stop_send_loop) {
        std::unique_lock ul(_send_queue_mx);
        _send_data_cv.wait(ul, [&](){
            return (_send_queue.size() > 0 && !_send_executing) || _stop_send_loop;
        });
        if (_stop_send_loop) {
            return;
        }
        this->execute_send();
    }
}

void network::execute_send() {
    _send_executing = true;
    boost::asio::mutable_buffer buf(&_send_queue.front(), sizeof(uint8_t));
    auto it = _send_queue.begin();

    boost::asio::async_write(_socket, buf, 
        [&, it](const boost::system::error_code& er, size_t){
            if (er) {
                std::cerr << "Send: " << er.message() << std::endl;
                _socket_connected = false;
                this->notify_disconnected();
                this->connect();
            }
            this->erase_el_from_queue(it);
            _send_executing = false;
            _send_data_cv.notify_all();
        });
}

void network::erase_el_from_queue(const std::list<uint8_t>::iterator& it) {
    std::lock_guard lg(_send_queue_mx);
    _send_queue.erase(it);
}
