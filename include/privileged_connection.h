#pragma once

#include <mutex>

#include "privileged_accept_task.h"
#include "privileged_server.h"

enum class privileged_serv_signals : int8_t {
    restart_game = -4,
    stop_game = -5,
    board_size = -6,
    clients_count = -7,
    snake_move_time = -8
};

class game_server;

class privileged_connection {
public:
    privileged_connection(std::unique_ptr<privileged_accept_task>&& accept_task);

    void attach_observer(game_server* observer);
    void update_server_accepted(const std::shared_ptr<privileged_server>& server);
    void update_disconnected();
    void update_data_received(const std::vector<int8_t>& data_received);

    void send_data(const std::vector<int8_t>& data);
    void send_clients_count(size_t count);

private:
    size_t decode_time_interval(const std::vector<int8_t>& time_interval) const;

    std::shared_ptr<privileged_server> _priv_server;
    std::mutex _server_mx;

    std::unique_ptr<privileged_accept_task> _accept_task_ptr;

    game_server* _game_observer = nullptr;
    std::mutex _observer_mx;
};
