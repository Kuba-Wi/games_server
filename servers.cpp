#include "servers.h"

servers::servers() : _server_endpoint(boost::asio::ip::tcp::v4(), 30000), 
                     _acceptor(_io_context, _server_endpoint),
                     _tmp_socket(_io_context) {

    accept_new_clients();
    _io_context_th = std::thread{[&](){
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
                    _server_list.emplace_back(std::make_unique<server>(socket));
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
