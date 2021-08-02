#include "client_connection.h"
#include "ui_iface.h"
#include <iostream>

client_connection::client_connection() : _socket(_io_context) {
    boost::system::error_code er_adress;
    boost::system::error_code er_connect;
    _server_endpoint.address(boost::asio::ip::make_address("localhost", er_adress));
    _server_endpoint.port(30000);
    if (er_adress) {
        std::cerr << "Address: " << er_adress.message() << "\n";
    }
    _socket.connect(_server_endpoint, er_connect);
    if (er_connect) {
        std::cerr << "Connect: " << er_connect.message() << "\n";
    }
    this->receive_data();

    _io_context_thread = std::thread{[&](){
        using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
        work_guard_type work_guard(_io_context.get_executor());
        _io_context.run();
    }};
}

client_connection::~client_connection() {
    _io_context.stop();
    _io_context_thread.join();
}

void client_connection::send_data(uint8_t data) {
    if (!_sending_data_enabled) {
        return;
    }
    try {
        std::unique_lock ul(_send_queue_mx);
        _send_queue.push_back(data);
        boost::asio::mutable_buffer buf(&_send_queue.back(), sizeof(uint8_t));
        auto it = std::prev(_send_queue.end());
        ul.unlock();
        boost::asio::async_write(_socket, buf, 
            [&, it](const boost::system::error_code& er, size_t) { 
                if (er) {
                    std::cerr << "Send: " << er.message() << std::endl;
                }
                std::lock_guard lg(_send_queue_mx);
                _send_queue.erase(it);
            });
    } catch (std::exception& e) {
        std::cerr << "Send exception: " << e.what() << std::endl;
    }
}

void client_connection::receive_data() {
    try {
        async_read_until(_socket,
            boost::asio::dynamic_buffer(_data_received), data_delimiter,
            [&](const boost::system::error_code& er, size_t bytes_with_delimiter) {
                if (!er) {
                    if (_data_received.front() < 0) {
                        this->process_received_signal(_data_received);
                    } else {
                        this->refresh_client_data(bytes_with_delimiter - 1);
                        refresh_client();
                    }
                    this->refresh_data_buffer(bytes_with_delimiter);
                    this->receive_data();
                } else {
                    std::cerr << "Receive: " << er.message() << std::endl;
                }
            });
    } catch (std::exception& e) {
        std::cerr << "Receive exception: " << e.what() << std::endl;
    }
}

void client_connection::process_received_signal(const std::vector<int8_t>& signal) {
    if (static_cast<client_signal>(signal.front()) == client_signal::start_sending) {
        _sending_data_enabled = true;
        enable_sending();
    } else if (static_cast<client_signal>(signal.front()) == client_signal::initial_data) {
        _board_height = signal[1];
        _board_width = signal[2];
        set_board_dimensions();
    } else if (static_cast<client_signal>(signal.front()) == client_signal::stop_sending) {
        _sending_data_enabled = false;
        stop_sending();
    }
}

bool client_connection::check_index_present(uint8_t x, uint8_t y) const {
    std::lock_guard lg(_client_data_mx);
    return std::any_of(_client_data.begin(), _client_data.end(), [x, y](const auto& pair){
            return pair.first == x && pair.second == y;
        });
}

void client_connection::refresh_client_data(size_t bytes_received) {
    std::lock_guard lg(_client_data_mx);
    _client_data.resize(bytes_received / sizeof(decltype(_client_data)::value_type));
    auto it = _client_data.begin();
    for (size_t i = 0; i < bytes_received - 1; i += 2) {
        *(it++) = {_data_received[i], _data_received[i + 1]};
    }
}

void client_connection::refresh_data_buffer(size_t bytes_with_delimiter) {
    _data_received.erase(_data_received.begin(), _data_received.begin() + bytes_with_delimiter);
}
