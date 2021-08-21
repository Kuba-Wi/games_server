#include "servers.h"

#include <spdlog/spdlog.h>

servers::servers() : _server_endpoint(boost::asio::ip::tcp::v4(), port_number), 
                     _acceptor(_io_context, _server_endpoint), 
                     _data_received_timer(_io_context) {}

servers::~servers() {
    std::unique_lock ul(_server_list_mx);
    _receiving_server.reset();
    _server_list.clear();
    ul.unlock();

    _servers_running = false;
    _data_received_timer.cancel();
    _io_context.stop();
    _io_context_th.join();
}

void servers::start_servers() {
    if (_servers_running) {
        return;
    }
    _servers_running = true;
    this->accept_new_clients();
    _io_context_th = std::thread{[&](){
        using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
        work_guard_type work_guard(_io_context.get_executor());
        _io_context.run();
    }};

    _data_received_timer.expires_at(boost::posix_time::pos_infin);
    _data_received_timer.async_wait([&](const boost::system::error_code&){
        this->check_timer();
    });
}

void servers::set_initial_data(const send_type& data) {
    _initial_data.resize(data.size() + 1);
    _initial_data.front() = static_cast<int8_t>(client_signal::initial_data);
    std::copy(data.begin(), data.end(), _initial_data.begin() + 1);
}

void servers::accept_new_clients() {
    _acceptor.async_accept(
        [&](boost::system::error_code er, boost::asio::ip::tcp::socket socket) {
            if (!er) {
                spdlog::info("New client accepted");
                this->add_accepted_server(std::move(socket));
                this->accept_new_clients();
            } else {
                spdlog::info("Accepting new client failed: {}", er.message());
            }
        });
}

void servers::add_accepted_server(boost::asio::ip::tcp::socket&& socket) {
    std::lock_guard lg(_server_list_mx);
    _server_list.emplace_back(std::make_shared<server>(std::move(socket), this));
    _server_list.back()->receive_data();
    this->send_initial_data(_server_list.back());
    if (_server_list.size() == 1) {
        this->update_receiving_serv();
    }
}

void servers::send_data(const send_type& data) {
    std::lock_guard lg(_server_list_mx);
    if (_server_list.size() > 0) {
        _receiving_server->send_data(data);
    }
}

void servers::change_receiving_server() {
    std::lock_guard lg(_server_list_mx);
    if (_server_list.size() <= 1) {
        return;
    }
    this->send_client_signal(client_signal::stop_sending);
    auto ptr = _receiving_server;
    _server_list.remove(_receiving_server);
    _server_list.push_back(ptr);
    this->update_receiving_serv();
}

void servers::update_receiving_serv() {
    _receiving_server = _server_list.front();
    this->send_client_signal(client_signal::start_sending);
    _data_received_timer.expires_from_now(boost::posix_time::seconds(timeout_seconds));
}

void servers::update_data_received(uint8_t byte_received) {
    if (_game_server_observer) {
        _game_server_observer->update_game(byte_received); 
    }
    _data_received_timer.expires_from_now(boost::posix_time::seconds(timeout_seconds));
}

void servers::update_disconnected(const std::shared_ptr<server>& disconnected) {
    this->remove_disconnected_serv(disconnected);
}

void servers::remove_disconnected_serv(const std::shared_ptr<server>& disconnected) {
    std::lock_guard lg(_server_list_mx);
    _server_list.remove(disconnected);

    if (_server_list.size() > 0) {
        if (_receiving_server == disconnected) {
            this->update_receiving_serv();
        }
    }
}

void servers::send_initial_data(const std::shared_ptr<server>& server_ptr) {
    server_ptr->send_data(_initial_data);
}

void servers::send_client_signal(client_signal signal) {
    _receiving_server->send_data({static_cast<int8_t>(signal)});
}

void servers::check_timer() {
    if (!_servers_running) {
        return;
    }

    if (_data_received_timer.expires_at() <= boost::asio::deadline_timer::traits_type::now()) {
        this->change_receiving_server();
        _data_received_timer.expires_from_now(boost::posix_time::seconds(timeout_seconds));
    }

    _data_received_timer.async_wait([&](const boost::system::error_code&){
        this->check_timer();
    });
}
