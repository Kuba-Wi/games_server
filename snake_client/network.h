#pragma once

#include <boost/asio.hpp>

#include <list>
#include <mutex>
#include <vector>

constexpr int8_t data_delimiter = std::numeric_limits<int8_t>::max();

class Isnake_client {
public:
    virtual ~Isnake_client() = default;
    virtual void update_snake(const std::vector<int8_t>& data, size_t bytes_received) = 0;
};

class network {
public:
    network();
    ~network();
    void connect();
    void receive_data();
    void send_data(uint8_t data);
    void attach_observer(Isnake_client* observer) { _snake_observer = observer; }

private:
    void refresh_data_buffer(size_t bytes_with_delimiter);
    void notify_snake(size_t bytes_received) { _snake_observer->update_snake(_data_received, bytes_received); }

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::endpoint _server_endpoint;
    boost::asio::ip::tcp::socket _socket;

    std::thread _io_context_thread;

    std::mutex _send_queue_mx;
    std::list<int8_t> _send_queue;
    std::vector<int8_t> _data_received;

    Isnake_client* _snake_observer = nullptr;
};
