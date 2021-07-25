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
    void remove_disconnected_serv();

private:
    bool clients_connected() const {
        return _server_list.size() > 0;
    }
    void update_receiving_serv();

    std::list<std::shared_ptr<server>> _server_list;
    server_iterator _receiving_server_it;

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::endpoint _server_endpoint;
    boost::asio::ip::tcp::acceptor _acceptor;

    std::thread _io_context_th;
    std::mutex _list_mx;
    std::atomic<int8_t> _data_received{0};
    std::unique_ptr<client_signal> _signal_to_send_ptr;
};
