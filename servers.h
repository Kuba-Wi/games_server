#pragma once

#include "server.h"

class servers {
public:
    using server_iterator = std::list<std::shared_ptr<server>>::iterator;
    servers();
    ~servers();
    void set_initial_data(const send_type& data);
    void accept_new_clients();
    void send_data(const send_type& data);
    std::optional<uint8_t> get_data_received();
    void remove_disconnected_serv();

private:
    bool clients_connected() const { return _server_list.size() > 0; }
    void update_receiving_serv();
    void add_accepted_server(boost::asio::ip::tcp::socket& socket);
    void send_initial_data(const std::shared_ptr<server>& server_ptr);

    std::list<std::shared_ptr<server>> _server_list;
    server_iterator _receiving_server_it;
    send_type _initial_data;

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::endpoint _server_endpoint;
    boost::asio::ip::tcp::acceptor _acceptor;

    std::thread _io_context_th;
    std::mutex _server_list_mx;
};
