#pragma once

#include <list>
#include <memory>

#include "server.h"

class servers {
public:
    servers();
    ~servers();
    void accept_new_clients();
    void send_data(const server::send_type& data);
    uint8_t get_data_received();

private:
    std::list<std::shared_ptr<server>> _server_list;

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::endpoint _server_endpoint;
    boost::asio::ip::tcp::acceptor _acceptor;

    std::thread _io_context_th;
    std::mutex _list_mx;
    std::atomic<uint8_t> _data_received{0};
};
