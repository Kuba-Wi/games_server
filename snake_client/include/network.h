#pragma once

#include <atomic>
#include <condition_variable>
#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <boost/asio.hpp>

#include "receive_worker.h"
#include "send_worker.h"

constexpr size_t port_number = 30000;

class snake_client;

class network {
public:
    network();
    ~network();
    void attach_observer(snake_client* observer);
    bool set_server_address(const std::string& ip);
    void connect();
    void send_data(uint8_t data);

    void update_data_received(const std::vector<int8_t>& received_data);
    void update_disconnected();

private:
    void notify_connected() const;

    boost::asio::io_context _io_context;
    std::list<tcp::endpoint> _server_endpoint;
    std::shared_ptr<tcp::socket> _socket_ptr;

    std::thread _io_context_th;
    std::atomic<bool> _socket_connected{false};
    std::atomic<bool> _address_set{false};

    receive_worker _receive_worker;
    send_worker _send_worker;

    snake_client* _snake_observer = nullptr;
    mutable std::mutex _observer_mx;
};
