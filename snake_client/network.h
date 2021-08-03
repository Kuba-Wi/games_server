#pragma once

#include <boost/asio.hpp>

#include <atomic>
#include <condition_variable>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

constexpr int8_t data_delimiter = std::numeric_limits<int8_t>::max();

class Isnake_client {
public:
    virtual ~Isnake_client() = default;
    virtual void update_snake(const std::vector<int8_t>& data, size_t bytes_received) = 0;
    virtual void set_disconnected() = 0;
    virtual void set_connected() = 0;
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
    void prepare_socket_connect() { _socket.close(); }

    void notify_update(size_t bytes_received) { _snake_observer->update_snake(_data_received, bytes_received); }
    void notify_disconnected() const { _snake_observer->set_disconnected(); }
    void notify_connected() const { _snake_observer->set_connected(); }

    void send_loop();
    void execute_send();
    void erase_el_from_queue(const std::list<uint8_t>::iterator& it);

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::endpoint _server_endpoint;
    boost::asio::ip::tcp::socket _socket;
    std::thread _io_context_thread;
    std::atomic<bool> _socket_connected{false};

    std::mutex _send_queue_mx;
    std::list<uint8_t> _send_queue;
    std::thread _send_loop_th;
    std::condition_variable _send_data_cv;
    std::atomic<bool> _sending_blocked{false};
    std::atomic<bool> _stop_send_loop{false};

    std::vector<int8_t> _data_received;

    Isnake_client* _snake_observer = nullptr;
};
