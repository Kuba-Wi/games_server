#pragma once

#include "server.h"

enum class client_signal : int8_t {
    start_sending = -1,
    initial_data = -2,
    stop_sending = -3
};

class game_server;

constexpr size_t port_number = 30000;
constexpr size_t timeout_seconds = 20;

class servers {
public:
    servers();
    ~servers();
    void start_servers();
    void set_initial_data(const send_type& data);
    void send_data(const send_type& data);
    void change_receiving_server();

    void attach_observer(game_server* observer) { _game_server_observer = observer; }
    void update_data_received(uint8_t byte_received);
    void update_disconnected(const std::shared_ptr<server>& disconnected);

private:
    void accept_new_clients();
    void remove_disconnected_serv(const std::shared_ptr<server>& disconnected);
    void update_receiving_serv();
    void add_accepted_server(boost::asio::ip::tcp::socket& socket);
    void send_initial_data(const std::shared_ptr<server>& server_ptr);
    void send_client_signal(client_signal signal);
    void check_timer();

    std::list<std::shared_ptr<server>> _server_list;
    std::mutex _server_list_mx;
    std::shared_ptr<server> _receiving_server;

    send_type _initial_data;
    std::atomic<bool> _servers_running{false};

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::endpoint _server_endpoint;
    boost::asio::ip::tcp::acceptor _acceptor;
    boost::asio::deadline_timer _data_received_timer;
    std::thread _io_context_th;

    game_server* _game_server_observer = nullptr;
};
