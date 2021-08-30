#pragma once

#include <boost/asio.hpp>

#include <thread>

using tcp = boost::asio::ip::tcp;

constexpr size_t port_number = 30000;

class servers;

class accept_task {
public:
    accept_task();
    ~accept_task();
    void attach_observer(servers* observer) { _servers_observer = observer; }

private:
    void accept_connections();
    void notify_client_accepted(tcp::socket& socket);

    boost::asio::io_context _io_context;
    tcp::endpoint _server_endpoint;
    tcp::acceptor _acceptor;
    std::thread _io_context_th;

    servers* _servers_observer = nullptr;
};
