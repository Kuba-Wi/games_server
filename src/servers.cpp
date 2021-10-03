#include "servers.h"

#include <spdlog/spdlog.h>

#include "game_server.h"

servers::servers(std::unique_ptr<accept_task>&& accept, std::unique_ptr<timeout_task>&& timeout) : 
        _accept_task_ptr(std::move(accept)),
        _timeout_task_ptr(std::move(timeout)) {

    _accept_task_ptr->attach_observer(this);
    _timeout_task_ptr->attach_observer(this);
}

servers::~servers() {
    std::lock_guard lg(_server_mx);
    _receiving_server.reset();
    _server_list.clear();
}

void servers::attach_observer(game_server* observer) {
    std::lock_guard lg(_observer_mx);
    _game_server_observer = observer; 
}

void servers::update_initial_data(const send_type& data) {
    std::scoped_lock sl(_initial_data_mx, _server_mx);
    _initial_data.resize(data.size() + 1);
    _initial_data.front() = static_cast<int8_t>(client_signal::initial_data);
    std::copy(data.begin(), data.end(), _initial_data.begin() + 1);

    for (auto& serv : _server_list) {
        this->send_initial_data(serv);
    }
}

void servers::update_server_accepted(const std::shared_ptr<server>& server) {
    size_t clients_count = 0;
    {
        std::scoped_lock sl(_server_mx, _initial_data_mx);
        _server_list.emplace_back(server);
        _server_list.back()->receive_data();
        this->send_initial_data(_server_list.back());

        if (_server_list.size() == 1) {
            this->update_receiving_serv();
        }
        clients_count = _server_list.size();
    }
    this->notify_clients_count_changed(clients_count);
}

void servers::send_data(const send_type& data) {
    std::lock_guard lg(_server_mx);
    if (_server_list.size() > 0) {
        _receiving_server->send_data(data);
    }
}

void servers::change_receiving_server() {
    std::lock_guard lg(_server_mx);
    if (_server_list.size() <= 1) {
        return;
    }
    this->send_client_signal(client_signal::stop_sending);
    auto ptr = _receiving_server;
    _server_list.remove(_receiving_server);
    _server_list.push_back(ptr);
    this->update_receiving_serv();
}

void servers::accept_connections() {
    _accept_task_ptr->accept_connections();
}

void servers::update_receiving_serv() {
    _receiving_server = _server_list.front();
    this->send_client_signal(client_signal::start_sending);
    _timeout_task_ptr->reset_deadline();
}

void servers::update_data_received(uint8_t byte_received) {
    std::unique_lock ul(_observer_mx);
    if (_game_server_observer) {
        _game_server_observer->update_game(byte_received); 
    }
    ul.unlock();

    _timeout_task_ptr->reset_deadline();
}

void servers::update_disconnected(const std::shared_ptr<server>& disconnected) {
    auto clients_count = this->remove_disconnected_serv(disconnected);
    this->notify_clients_count_changed(clients_count);
}

size_t servers::remove_disconnected_serv(const std::shared_ptr<server>& disconnected) {
    std::lock_guard lg(_server_mx);
    _server_list.remove(disconnected);

    if (_server_list.size() == 0) {
        _receiving_server.reset();
    } else if (_receiving_server == disconnected) {
        this->update_receiving_serv();
    }

    return _server_list.size();
}

void servers::send_initial_data(const std::shared_ptr<server>& server_ptr) {
    if (_initial_data.size() > 0) {
        server_ptr->send_data(_initial_data);
    }
}

void servers::send_client_signal(client_signal signal) {
    _receiving_server->send_data({static_cast<int8_t>(signal)});
}

size_t servers::get_clients_count() {
    std::lock_guard lg(_server_mx);
    return _server_list.size();
}

void servers::notify_clients_count_changed(size_t clients_count) {
    std::lock_guard lg(_observer_mx);
    if (_game_server_observer) {
        _game_server_observer->update_clients_count_changed(clients_count);
    }
}
