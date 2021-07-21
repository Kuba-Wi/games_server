#pragma once

#include <list>
#include <memory>

#include "server.h"

class servers {
public:
    using server_iterator = std::list<std::shared_ptr<server>>::iterator;
    servers();
    ~servers();
    void accept_new_clients();
    void send_data(const send_type& data);
    uint8_t get_data_received();

private:
    void update_receiving_serv();
    void remove_disconnected_serv();

    std::list<std::shared_ptr<server>> _server_list;
    server_iterator _receiving_server_it;

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::endpoint _server_endpoint;
    boost::asio::ip::tcp::acceptor _acceptor;

    std::thread _io_context_th;
    std::mutex _list_mx;
    std::atomic<uint8_t> _data_received{0};
};
