#pragma once

#include <boost/asio.hpp>

#include <atomic>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

class server : public std::enable_shared_from_this<server> {
public:
    using send_type = std::vector<std::pair<uint8_t, uint8_t>>;
    using send_iterator = std::list<server::send_type>::iterator;

    server() = delete;
    server(boost::asio::ip::tcp::socket& socket);
    void send_data(const send_type& data);
    bool is_socket_connected() { return _socket_connected; }
    void end_connection() { _socket_connected = false; }
    void erase_el_from_queue(const send_iterator& it);

    uint8_t get_received_data() const;
private:
    void receive_data();
    boost::asio::ip::tcp::socket _socket;
    std::atomic<bool> _socket_connected{true};

    uint8_t _data_buffer;
    std::atomic<uint8_t> _byte_received{0};

    std::list<send_type> _send_queue;
    std::mutex _send_mutex;
};

struct send_handler {
    send_handler(const std::shared_ptr<server>& ptr, const server::send_iterator& iter) : server_ptr{ptr}, it{iter} {}
    void operator()(const boost::system::error_code& er, size_t) {
        if (er) {
            server_ptr->end_connection();
            std::cerr << er.message() << std::endl;
        }
        server_ptr->erase_el_from_queue(it);
    }

    std::shared_ptr<server> server_ptr;    
    server::send_iterator it;
};
