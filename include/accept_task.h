#pragma once

#include <boost/asio.hpp>

#include <mutex>
#include <thread>

#include "test_iface.h"

using tcp = boost::asio::ip::tcp;

constexpr size_t port_number = 30000;

class server;
class servers;

class accept_task {
public:
    accept_task();
    TEST_IFACE ~accept_task();
    TEST_IFACE void attach_observer(servers* observer);

private:
    void accept_connections();
    void notify_client_accepted(tcp::socket& socket);

    boost::asio::io_context _io_context;
    tcp::endpoint _server_endpoint;
    tcp::acceptor _acceptor;
    std::thread _io_context_th;

    servers* _servers_observer = nullptr;
    std::mutex _observer_mx;
};
