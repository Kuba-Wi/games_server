#include "servers.h"

servers::servers() : _server_endpoint(boost::asio::ip::tcp::v4(), port_number), 
                     _acceptor(_io_context, _server_endpoint) {}

servers::~servers() {
    _io_context.stop();
    _io_context_th.join();
    _servers_running = false;
    _remove_disconnected_th.join();
}

void servers::start_servers() {
    if (_servers_running || !_game_server_observer) {
        return;
    }
    _servers_running = true;
    this->accept_new_clients();
    _io_context_th = std::thread{[&](){
        using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
        work_guard_type work_guard(_io_context.get_executor());
        _io_context.run();
    }};

    _remove_disconnected_th = std::thread{[&](){
        this->remove_loop();
    }};
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
                this->add_accepted_server(socket);
                this->accept_new_clients();
            } else {
                std::cerr << "Accept: " << er.message() << std::endl;
            }
        });
}

void servers::add_accepted_server(boost::asio::ip::tcp::socket& socket) {
    std::lock_guard lg(_server_list_mx);
    _server_list.emplace_back(std::make_shared<server>(socket, this));
    _server_list.back()->receive_data();
    this->send_initial_data(_server_list.back());
    if (_server_list.size() == 1) {
        this->update_receiving_it();
    }
}

void servers::send_data(const send_type& data) {
    std::lock_guard lg(_server_list_mx);
    if (this->clients_connected()) {
        (*_receiving_server_it)->send_data(data);
    }
}

void servers::change_receiving_server() {
    std::lock_guard lg(_server_list_mx);
    if (_server_list.size() <= 1) {
        return;
    }
    this->send_client_signal(client_signal::stop_sending);
    std::shared_ptr<server> ptr = *_receiving_server_it;
    _server_list.erase(_receiving_server_it);
    _server_list.push_back(ptr);
    this->update_receiving_it();
}

void servers::update_receiving_it() {
    if (this->clients_connected()) {
        if (_receiving_server_it != _server_list.begin()) {
            _receiving_server_it = _server_list.begin();
            this->send_client_signal(client_signal::start_sending);
        }
    }
}

void servers::remove_disconnected_serv() {
    std::lock_guard lg(_server_list_mx);
    size_t elements_removed = _server_list.remove_if([&](auto& serv){
            return !serv->is_socket_connected();
        });
    if (elements_removed > 0) {
        this->update_receiving_it();
    }
}

void servers::remove_loop() {
    constexpr size_t sleep_time = 100;
    while (_servers_running) {
        this->remove_disconnected_serv();
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }
}

void servers::send_initial_data(const std::shared_ptr<server>& server_ptr) {
    server_ptr->send_data(_initial_data);
}

void servers::send_client_signal(client_signal signal) {
    (*_receiving_server_it)->send_data({static_cast<int8_t>(signal)});
}
