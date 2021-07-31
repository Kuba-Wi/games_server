#pragma once

#include <boost/asio.hpp>

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

using send_type = std::vector<int8_t>;
using send_iterator = std::list<send_type>::iterator;

class server : public std::enable_shared_from_this<server> {
public:
    server(boost::asio::ip::tcp::socket& socket);
    ~server();
    void receive_data();
    void send_data(const send_type& data);
    bool is_socket_connected() { return _socket_connected; }
    void end_connection() { _socket_connected = false; }
    void erase_el_from_queue(const send_iterator& it);

    std::optional<uint8_t> get_received_data();
    void update_byte_received();
private:
    void execute_send();
    void send_loop();

    boost::asio::ip::tcp::socket _socket;
    std::atomic<bool> _socket_connected{true};
    std::atomic<bool> _sending_blocked{false};

    uint8_t _data_buffer;
    std::optional<uint8_t> _byte_received;
    std::mutex _byte_received_mx;

    std::list<send_type> _send_queue;
    std::mutex _send_mx;
    std::condition_variable _send_data_cv;
    std::thread _execute_send_th;
};
