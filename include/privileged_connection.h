#pragma once

#include <mutex>

#include "privileged_accept_task.h"
#include "privileged_server.h"

enum class privileged_serv_signals : int8_t {
    restart_game = -4,
    stop_game = -5,
    board_size = -6,
    get_clients_count = -7
};

class privileged_connection {
public:
    privileged_connection(std::unique_ptr<privileged_accept_task>&& accept_task);

    void update_server_accepted(const std::shared_ptr<privileged_server>& server);
    void update_disconnected();
    void update_data_received(const std::vector<int8_t>& data_received);

private:
    std::shared_ptr<privileged_server> _priv_server;
    std::mutex _server_mx;

    std::unique_ptr<privileged_accept_task> _accept_task_ptr;
};
