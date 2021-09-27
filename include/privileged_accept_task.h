#pragma once

#include <mutex>
#include <thread>

#include <boost/asio.hpp>

#include "test_iface.h"

using tcp = boost::asio::ip::tcp;

constexpr size_t privileged_port_number = 30001;

class privileged_connection;

class privileged_accept_task {
public:
    privileged_accept_task();
    TEST_IFACE ~privileged_accept_task();
    TEST_IFACE void attach_observer(privileged_connection* observer);
    TEST_IFACE void accept_connections();

private:
    void notify_client_accepted(tcp::socket& socket);

    boost::asio::io_context _io_context;
    tcp::endpoint _server_endpoint;
    tcp::acceptor _acceptor;
    std::thread _io_context_th;

    privileged_connection* _connection_observer = nullptr;
    std::mutex _observer_mx;
};
