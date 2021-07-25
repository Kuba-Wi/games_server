#pragma once

#include <boost/asio.hpp>

#include <atomic>
#include <concepts>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

enum class client_signal : int8_t {
    start_sending = -1
};

using send_type = std::vector<std::pair<uint8_t, uint8_t>>;
using send_iterator = std::list<send_type>::iterator;
using signal_iterator = std::list<int8_t>::iterator;

template <typename T>
concept send_or_signal_iterator = (std::same_as<T, send_iterator> || std::same_as<T, signal_iterator>);

class server : public std::enable_shared_from_this<server> {
public:
    server() = delete;
    server(boost::asio::ip::tcp::socket& socket) : _socket{std::move(socket)} {}
    void receive_data();
    void send_data(const send_type& data);
    void send_client_signal(client_signal signal);
    bool is_socket_connected() { return _socket_connected; }
    void end_connection() { _socket_connected = false; }

    template <typename iterator_type>
    requires send_or_signal_iterator<iterator_type>
    void erase_el_from_queue(const iterator_type& it);

    int8_t get_received_data() const;
    void update_byte_received();
private:
    template <typename iterator_type>
    requires send_or_signal_iterator<iterator_type>
    void execute_send(const boost::asio::mutable_buffer& buf, const iterator_type& it);

    boost::asio::ip::tcp::socket _socket;
    std::atomic<bool> _socket_connected{true};

    int8_t _data_buffer;
    std::atomic<int8_t> _byte_received{0};

    std::list<send_type> _send_queue;
    std::mutex _send_mutex;

    std::list<int8_t> _signal_queue;
    std::mutex _signal_mutex;
};

template <typename iterator_type>
requires send_or_signal_iterator<iterator_type>
void server::erase_el_from_queue(const iterator_type& it) {
    if constexpr (std::is_same_v<iterator_type, send_iterator>) {
        std::lock_guard lg(_send_mutex);
        _send_queue.erase(it);
    } else {
        std::lock_guard lg(_signal_mutex);
        _signal_queue.erase(it);
    }
}

template <typename iterator_type>
requires send_or_signal_iterator<iterator_type>
void server::execute_send(const boost::asio::mutable_buffer& buf, const iterator_type& it) {
    try {
        boost::asio::async_write(_socket, buf, 
            [it, ptr = this->shared_from_this()](const boost::system::error_code& er, size_t){
                if (er) {
                    ptr->end_connection();
                    std::cerr << "Send: " << er.message() << std::endl;
                }
                ptr->erase_el_from_queue(it);
            });
    } catch (std::exception& e) {
        _socket_connected = false;
        std::cerr << "Send exception: " << e.what() << std::endl;
    }
}
