#include "network.h"
#include "snake_client.h"
#include "socket_option.h"

network::network() : _socket_ptr(std::make_shared<tcp::socket>(_io_context)), 
                     _receive_worker(_socket_ptr, this), _send_worker(_socket_ptr) {
    _server_endpoint.emplace_back(boost::asio::ip::tcp::endpoint{});
    _server_endpoint.back().port(port_number);

    _io_context_th = std::thread{[&](){
        using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
        work_guard_type work_guard(_io_context.get_executor());
        _io_context.run();
    }};
}

network::~network() {
    _io_context.stop();
    _io_context_th.join();
}

void network::attach_observer(snake_client* observer) {
    std::lock_guard lg(_observer_mx);
    _snake_observer = observer; 
}

bool network::set_server_address(const std::string& ip) {
    boost::system::error_code er_adress;
    auto address = boost::asio::ip::make_address(ip, er_adress);
    if (er_adress) {
        return false;
    }
    _server_endpoint.back().address(address);
    _address_set = true;
    return true;
}

void network::connect() {
    if (!_address_set || _socket_connected) {
        return;
    }

    boost::asio::async_connect(*_socket_ptr, _server_endpoint, 
        [&](const boost::system::error_code& error, const boost::asio::ip::tcp::endpoint&){
            if (error) {
                this->notify_connection_failed(error.message());
            } else {
                _socket_connected = true;
                set_socket_no_delay_option(*_socket_ptr);
                this->notify_connected();
                _receive_worker.start_receive_data();
            }
    });
}

void network::send_data(uint8_t data) {
    if (!_socket_connected) {
        return;
    }

    _send_worker.send_data(data);
}

void network::update_data_received(const std::vector<int8_t>& received_data) {
    std::lock_guard lg(_observer_mx);
    if (_snake_observer) {
        _snake_observer->update_snake(received_data); 
    }
}

void network::update_disconnected(const std::string& message) {
    std::unique_lock ul(_observer_mx);
    if (_snake_observer) {
        _snake_observer->update_disconnected(message);
    }
    ul.unlock();

    _socket_connected = false;
}

void network::notify_connected() const {
    std::lock_guard lg(_observer_mx);
    if (_snake_observer) {
        _snake_observer->set_connected();
    }
}

void network::notify_connection_failed(const std::string& message) const {
    std::lock_guard lg(_observer_mx);
    if (_snake_observer) {
        _snake_observer->update_connection_failed(message);
    }
}
