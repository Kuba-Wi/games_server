#include "servers.h"

servers::servers() : _server_endpoint(boost::asio::ip::tcp::v4(), 30000), 
                     _acceptor(_io_context, _server_endpoint) {

    accept_new_clients();
    _io_context_th = std::thread{[&](){
        using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
        work_guard_type work_guard(_io_context.get_executor());
        _io_context.run();
    }};
}

servers::~servers() {
    _io_context.stop();
    _io_context_th.join();
}

std::optional<uint8_t> servers::get_data_received() {
    std::lock_guard sl(_list_mx);
    if (clients_connected()) {
        return (*_receiving_server_it)->get_received_data();
    }
    return std::nullopt;
}

void servers::accept_new_clients() {
    try {
        _acceptor.async_accept(
            [&](boost::system::error_code er, boost::asio::ip::tcp::socket socket) {
                if (!er) {
                    std::unique_lock ul(_list_mx);
                    _server_list.emplace_back(std::make_shared<server>(socket));
                    _server_list.back()->receive_data();
                    this->update_receiving_serv();
                    ul.unlock();
                    accept_new_clients();
                } else {
                    std::cerr << "Accept: " << er.message() << std::endl;
                }
            });
    } catch(std::exception& e) {
        std::cerr << "Accept exception: " << e.what() << std::endl;
    }
}

void servers::send_data(const send_type& data) {
    std::lock_guard lg(_list_mx);
    if (clients_connected()) {
        (*_receiving_server_it)->send_data(data);
    }
}

void servers::update_receiving_serv() {
    if (clients_connected()) {
        if (_receiving_server_it != _server_list.begin()) {
            _receiving_server_it = _server_list.begin();
            (*_receiving_server_it)->send_client_signal(client_signal::start_sending);
        }
    }
}

void servers::remove_disconnected_serv() {
    std::lock_guard lg(_list_mx);
    _server_list.remove_if([&](auto& serv){
            return !serv->is_socket_connected();
        });
    this->update_receiving_serv();
}
