#include "privileged_connection.h"

privileged_connection::privileged_connection(std::unique_ptr<privileged_accept_task>&& accept_task) :
                                             _accept_task_ptr(std::move(accept_task)) {
    _accept_task_ptr->attach_observer(this);
    _accept_task_ptr->accept_connections();
}

void privileged_connection::update_server_accepted(const std::shared_ptr<privileged_server>& server) {
    std::lock_guard lg(_server_mx);
    _priv_server = server;
    _priv_server->receive_data();
}

void privileged_connection::update_disconnected() {
    std::lock_guard lg(_server_mx);
    _priv_server.reset();
}

void privileged_connection::update_data_received(const std::vector<int8_t>& data_received) {
    if (data_received.size() == 0) {
        return;
    }
    using signal = privileged_serv_signals;
    //TODO(kubaw): fill cases
    switch(static_cast<signal>(data_received.front())) {
    case signal::restart_game:
        break;
    case signal::stop_game:
        break;
    case signal::get_clients_count:
        break;
    case signal::board_size:
        break;
    }
}
