#pragma once

#include <boost/asio.hpp>

#include <atomic>
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

class server;

constexpr int8_t data_delimiter = std::numeric_limits<int8_t>::max();

using send_type = std::vector<int8_t>;
using send_iterator = std::list<send_type>::iterator;

class Iservers {
public:
    virtual ~Iservers() = default;
    virtual void update_data_received(uint8_t byte_received) = 0;
    virtual void update_disconnected(const std::shared_ptr<server>& disconnected) = 0;
};

class server : public std::enable_shared_from_this<server> {
public:
    server(boost::asio::ip::tcp::socket&& socket, Iservers* servers);
    ~server();
    void receive_data();
    void send_data(const send_type& data);
    void end_connection() { _socket_connected = false; }

private:
    void erase_el_from_queue(const send_iterator& it);
    void execute_send();
    void send_loop();
    void notify_data_received() const { _servers_observer->update_data_received(_data_buffer); }
    void notify_server_disconnected() { _servers_observer->update_disconnected(this->shared_from_this()); }

    boost::asio::ip::tcp::socket _socket;
    std::atomic<bool> _socket_connected{true};
    std::atomic<bool> _send_executing{false};

    uint8_t _data_buffer;
    std::list<send_type> _send_queue;
    const size_t _queue_max_size = 1000;
    std::mutex _send_mx;
    std::condition_variable _send_data_cv;
    std::thread _send_loop_th;

    Iservers* _servers_observer;
};
