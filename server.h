#pragma once

#include <boost/asio.hpp>

#include <vector>

#include "send_task.h"

class server;

class Iservers {
public:
    virtual ~Iservers() = default;
    virtual void update_data_received(uint8_t byte_received) = 0;
    virtual void update_disconnected(const std::shared_ptr<server>& disconnected) = 0;
};

class server : public std::enable_shared_from_this<server> {
public:
    server(boost::asio::ip::tcp::socket&& socket, Iservers* servers);
    void receive_data();
    void send_data(const send_type& data);

private:
    void notify_data_received() const { _servers_observer->update_data_received(_data_buffer); }
    void notify_server_disconnected() { _servers_observer->update_disconnected(this->shared_from_this()); }

    boost::asio::ip::tcp::socket _socket;
    uint8_t _data_buffer;

    Iservers* _servers_observer;
    std::shared_ptr<send_task> _send_task_ptr;
};
