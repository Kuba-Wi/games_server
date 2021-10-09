#pragma once

#include <mutex>
#include <thread>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "test_iface.h"

using boost::asio::ip::tcp;
using ssl_socket = boost::asio::ssl::stream<tcp::socket>;

constexpr size_t privileged_port_number = 30001;

class privileged_connection;

class privileged_accept_task {
public:
    privileged_accept_task();
    TEST_IFACE ~privileged_accept_task();
    TEST_IFACE void attach_observer(privileged_connection* observer);
    TEST_IFACE void accept_connections();

private:
    void do_handshake(const std::shared_ptr<ssl_socket>& socket);
    void notify_client_accepted(const std::shared_ptr<ssl_socket>& socket);

    tcp::endpoint _server_endpoint;
    boost::asio::io_context _io_context;
    boost::asio::ssl::context _ssl_context;
    tcp::acceptor _acceptor;
    std::thread _io_context_th;

    privileged_connection* _connection_observer = nullptr;
    std::mutex _observer_mx;
};
