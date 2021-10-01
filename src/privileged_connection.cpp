#include "privileged_connection.h"

#include "game_server.h"

privileged_connection::privileged_connection(std::unique_ptr<privileged_accept_task>&& accept_task) :
                                             _accept_task_ptr(std::move(accept_task)) {
    _accept_task_ptr->attach_observer(this);
    _accept_task_ptr->accept_connections();
}

void privileged_connection::attach_observer(game_server* observer) {
    std::lock_guard lg(_observer_mx);
    _game_observer = observer;
}

void privileged_connection::update_server_accepted(const std::shared_ptr<privileged_server>& server) {
    std::unique_lock ul_server(_server_mx);
    _priv_server = server;
    _priv_server->receive_data();
    ul_server.unlock();

    std::unique_lock ul_observer(_observer_mx);
    auto clients_count = _game_observer->get_clients_count();
    ul_observer.unlock();
    this->send_clients_count(clients_count);
}

void privileged_connection::update_disconnected() {
    std::unique_lock ul(_server_mx);
    _priv_server.reset();
    ul.unlock();

    std::lock_guard lg(_observer_mx);
    if (_game_observer) {
        _game_observer->stop_game();
    }
}

void privileged_connection::update_data_received(const std::vector<int8_t>& data_received) {
    if (data_received.size() == 0) {
        return;
    }
    using signal = privileged_serv_signals;
    
    std::unique_lock ul(_observer_mx);
    if (!_game_observer) {
        return;
    }

    switch(static_cast<signal>(data_received.front())) {
    case signal::restart_game:
        _game_observer->restart_game();
        break;
    case signal::stop_game:
        _game_observer->stop_game();
        break;
    case signal::board_size:
        if (data_received.size() >= 3) {
            _game_observer->update_board_size(data_received[1], data_received[2]);
        }
        break;
    }
}

void privileged_connection::send_data(const std::vector<int8_t>& data) {
    std::unique_lock ul(_server_mx);
    if (_priv_server)
        _priv_server->send_data(data);
}

void privileged_connection::send_clients_count(size_t count) {
    std::unique_lock ul(_server_mx);
    if (_priv_server)
        _priv_server->send_large_number(count, static_cast<int8_t>(privileged_serv_signals::clients_count));
}
