#pragma once

#include <boost/asio.hpp>

#include <atomic>
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "data_receiver.h"

constexpr int8_t data_delimiter = std::numeric_limits<int8_t>::max();
constexpr size_t port_number = 30000;

class Isnake_client {
public:
    virtual ~Isnake_client() = default;
    virtual void update_snake(const std::vector<int8_t>& data) = 0;
    virtual void set_disconnected() = 0;
    virtual void set_connected() = 0;
};

class network : public Inetwork {
public:
    network();
    ~network();
    void update_data_received(const std::vector<int8_t>& data) override;
    void attach_observer(Isnake_client* observer) { _snake_observer = observer; }
    bool set_server_address(const std::string& ip);
    void connect();
    void send_data(uint8_t data);

private:
    void start_data_receiver();
    void receive_signal();
    void refresh_data_buffer(size_t bytes_with_delimiter);
    void pass_received_data(size_t size) const;

    void notify_update(const std::vector<int8_t>& data_received) const;
    void notify_disconnected() const;
    void notify_connected() const;

    void send_loop();
    void execute_send();
    void erase_from_send_queue(const std::list<uint8_t>::iterator& it);

    void set_no_delay_option();

    boost::asio::io_context _io_context;
    std::list<boost::asio::ip::tcp::endpoint> _server_endpoint;
    boost::asio::ip::tcp::socket _socket;

    std::thread _io_context_th;
    std::atomic<bool> _socket_connected{false};
    std::atomic<bool> _address_set{false};
    std::atomic<bool> _send_executing{false};
    std::atomic<bool> _stop_network{false};

    std::mutex _send_queue_mx;
    std::list<uint8_t> _send_queue;
    std::thread _send_loop_th;
    std::condition_variable _send_data_cv;

    std::vector<int8_t> _data_received;

    Isnake_client* _snake_observer = nullptr;
    std::unique_ptr<data_receiver> _receiver_ptr;
};
