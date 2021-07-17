#pragma once

#include <boost/asio.hpp>

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

class server {
public:
    server() = delete;
    server(boost::asio::ip::tcp::socket& socket);
    void receive_data();
    template <typename T>
    void send_data(const std::pair<std::vector<T>, size_t>& data);
    bool is_socket_connected() {
        return _socket_connected;
    }

    uint8_t get_received_data() const;
private:
    boost::asio::ip::tcp::socket _socket;
    std::atomic<bool> _socket_connected{true};

    std::thread _io_context_thread;
    uint8_t _data_buffer;
    std::atomic<uint8_t> _byte_received{0};
    std::vector<std::pair<uint8_t, uint8_t>> _data_to_send;

    std::mutex _send_mutex;
};

template <typename T>
void server::send_data(const std::pair<std::vector<T>, size_t>& data) {
    std::unique_lock ul(_send_mutex);
    _data_to_send = data.first;
    boost::asio::mutable_buffer buf(_data_to_send.data(), _data_to_send.size() * sizeof(T));
    try {
        _socket.async_send(buf, 
            [&, ul = std::move(ul)](const boost::system::error_code& er, size_t) mutable { 
                if (er) {
                    _socket_connected = false;
                    std::cout << er.message() << std::endl;
                }
                ul.unlock();
            });
    } catch (std::exception& e) {
        _socket_connected = false;
        std::cerr << e.what() << std::endl;
    }
}
