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

uint8_t servers::get_data_received() {
    std::lock_guard sl(_list_mx);
    if (_server_list.size() > 0) {
        auto data = (*_receiving_server_it)->get_received_data();
        if (data < 0) {
            _signal_to_send_ptr.reset();
        } else {
            _data_received = data;
        }
    }
    return _data_received;
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
                    std::cerr << er.message() << std::endl;
                }
            });
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void servers::send_data(const send_type& data) {
    std::unique_lock ul(_list_mx);
    for (auto it = _server_list.begin(); it != _server_list.end(); ++it) {
        if (_signal_to_send_ptr && it == _receiving_server_it) {
            (*_receiving_server_it)->send_client_signal(*_signal_to_send_ptr);
        } else {
            (*it)->send_data(data);
        }
    }
    ul.unlock();

    this->remove_disconnected_serv();
}

void servers::update_receiving_serv() {
    if (_server_list.size() > 0) {
        if (_receiving_server_it != _server_list.begin()) {
            _receiving_server_it = _server_list.begin();
            _signal_to_send_ptr = std::make_unique<client_signal>(client_signal::start_sending);
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
