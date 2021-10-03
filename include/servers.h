#pragma once

#include "accept_task.h"
#include "server.h"
#include "test_iface.h"
#include "timeout_task.h"

enum class client_signal : int8_t {
    start_sending = -1,
    initial_data = -2,
    stop_sending = -3
};

class game_server;

class servers {
public:
    servers(std::unique_ptr<accept_task>&& accept, std::unique_ptr<timeout_task>&& timeout);
    TEST_IFACE ~servers();
    TEST_IFACE void attach_observer(game_server* observer);

    TEST_IFACE void update_initial_data(const send_type& data);
    TEST_IFACE void send_data(const send_type& data);
    TEST_IFACE void change_receiving_server();
    void accept_connections();

    void update_data_received(uint8_t byte_received);
    void update_disconnected(const std::shared_ptr<server>& disconnected);
    void update_server_accepted(const std::shared_ptr<server>& server);
    void update_timeout() { this->change_receiving_server(); }

    size_t get_clients_count();

private:
    size_t remove_disconnected_serv(const std::shared_ptr<server>& disconnected);
    void update_receiving_serv();
    void send_initial_data(const std::shared_ptr<server>& server_ptr);
    void send_client_signal(client_signal signal);
    void notify_clients_count_changed(size_t clients_count);

    std::list<std::shared_ptr<server>> _server_list;
    std::shared_ptr<server> _receiving_server;
    std::mutex _server_mx;

    send_type _initial_data;
    std::mutex _initial_data_mx;

    game_server* _game_server_observer = nullptr;
    std::mutex _observer_mx;

    std::unique_ptr<accept_task> _accept_task_ptr;
    std::unique_ptr<timeout_task> _timeout_task_ptr;
};
