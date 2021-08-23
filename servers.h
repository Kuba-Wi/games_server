#pragma once

#include "server.h"

enum class client_signal : int8_t {
    start_sending = -1,
    initial_data = -2,
    stop_sending = -3
};

class Iservers_observer {
public:
    virtual ~Iservers_observer() = default;
    virtual void update_game(uint8_t byte_received) = 0;
};

constexpr size_t port_number = 30000;
constexpr size_t timeout_seconds = 20;
constexpr size_t run_pool_size = 2;

class servers : public Iservers {
public:
    servers();
    ~servers();
    void start_servers();
    void set_initial_data(const send_type& data);
    void accept_new_clients();
    void send_data(const send_type& data);
    void change_receiving_server();
    
    void attach_observer(Iservers_observer* observer) { _game_server_observer = observer; }
    void update_data_received(uint8_t byte_received);
    void update_disconnected(const std::shared_ptr<server>& disconnected) override;

private:
    void remove_disconnected_serv(const std::shared_ptr<server>& disconnected);
    void update_receiving_serv();
    void add_accepted_server(boost::asio::ip::tcp::socket&& socket);
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
    std::vector<std::thread> _run_thread_pool;

    Iservers_observer* _game_server_observer = nullptr;
};
