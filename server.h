#pragma once

#include <boost/asio.hpp>

#include <atomic>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

enum class client_signal : int8_t {
    start_sending = -1
};

class server : public std::enable_shared_from_this<server> {
public:
    using send_type = std::vector<std::pair<uint8_t, uint8_t>>;
    using send_iterator = std::list<server::send_type>::iterator;
    using signal_iterator = std::list<int8_t>::iterator;

    server() = delete;
    server(boost::asio::ip::tcp::socket& socket) : _socket{std::move(socket)} {}
    void receive_data();
    void send_data(const send_type& data);
    void send_client_signal(client_signal signal);
    bool is_socket_connected() { return _socket_connected; }
    void end_connection() { _socket_connected = false; }
    template <typename iterator_type>
    void erase_el_from_queue(const iterator_type& it);

    uint8_t get_received_data() const;
    void update_byte_received();
private:
    template <typename iterator_type>
    void execute_send(const boost::asio::mutable_buffer& buf, const iterator_type& it);

    boost::asio::ip::tcp::socket _socket;
    std::atomic<bool> _socket_connected{true};

    uint8_t _data_buffer;
    std::atomic<uint8_t> _byte_received{0};

    std::list<send_type> _send_queue;
    std::mutex _send_mutex;

    std::list<int8_t> _signal_queue;
    std::mutex _signal_mutex;
};

template <typename iterator_type>
void server::erase_el_from_queue(const iterator_type& it) {
    static_assert(std::is_same_v<iterator_type, send_iterator> ||
                  std::is_same_v<iterator_type, signal_iterator>);

    if constexpr (std::is_same_v<iterator_type, send_iterator>) {
        std::lock_guard lg(_send_mutex);
        _send_queue.erase(it);
    } else {
        std::lock_guard lg(_signal_mutex);
        _signal_queue.erase(it);
    }
}

template <typename iterator_type, typename = typename std::enable_if_t<
                std::is_same_v<iterator_type, server::send_iterator> ||
                std::is_same_v<iterator_type, server::signal_iterator>>>
struct send_handler {
    send_handler(const std::shared_ptr<server>& ptr, const iterator_type& iter) : server_ptr{ptr}, it{iter} {}
    void operator()(const boost::system::error_code& er, size_t) {
        if (er) {
            server_ptr->end_connection();
            std::cerr << er.message() << std::endl;
        }
        server_ptr->erase_el_from_queue(it);
    }

    std::shared_ptr<server> server_ptr;    
    iterator_type it;
};

template <typename iterator_type>
void server::execute_send(const boost::asio::mutable_buffer& buf, const iterator_type& it) {
    send_handler<iterator_type> handler{this->shared_from_this(), it};
    
    try {
        boost::asio::async_write(_socket, buf, handler);
    } catch (std::exception& e) {
        _socket_connected = false;
        std::cerr << e.what() << std::endl;
    }
}
