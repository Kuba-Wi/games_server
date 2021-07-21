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
        _data_received = _server_list.front()->get_received_data();
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

void servers::send_data(const server::send_type& data) {
    std::unique_lock ul(_list_mx);
    auto it_end = _server_list.end();
    ul.unlock();
    for (auto it = _server_list.begin(); it != it_end; ++it) {
        (*it)->send_data(data);
    }

    ul.lock();
    _server_list.remove_if([&](auto& serv){
            return !serv->is_socket_connected();
        });
    ul.unlock();
}
