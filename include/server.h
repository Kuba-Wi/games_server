#pragma once

#include <vector>

#include <boost/asio.hpp>

#include "send_task.h"
#include "test_iface.h"

class servers;

class server : public std::enable_shared_from_this<server> {
public:
    server(tcp::socket&& socket, servers* servers);
    TEST_IFACE ~server() { _send_task_ptr->stop_task(); }
    TEST_IFACE void receive_data();
    TEST_IFACE void send_data(const send_type& data);

private:
    void notify_data_received() const;
    void notify_server_disconnected();

    std::shared_ptr<tcp::socket> _socket;
    uint8_t _data_buffer;

    servers* _servers_observer;
    std::shared_ptr<send_task> _send_task_ptr;
};
