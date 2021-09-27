#include "privileged_accept_task.h"

#include <spdlog/spdlog.h>

#include "privileged_connection.h"

privileged_accept_task::privileged_accept_task() : _server_endpoint(boost::asio::ip::tcp::v4(), privileged_port_number), 
                                                   _acceptor(_io_context, _server_endpoint.protocol()) {

    boost::system::error_code er;
    _acceptor.bind(_server_endpoint, er);
    if (er) {
        spdlog::error("Acceptor bind failed: {}. Terminate", er.message());
        std::terminate();
    }
    _acceptor.listen(boost::asio::socket_base::max_listen_connections, er);
    if (er) {
        spdlog::error("Acceptor listen failed: {}. Terminate", er.message());
        std::terminate();
    }

    _io_context_th = std::thread{[&](){
        using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
        work_guard_type work_guard(_io_context.get_executor());
        _io_context.run();
    }};
}

privileged_accept_task::~privileged_accept_task() {
    _io_context.stop();
    _io_context_th.join();
}

void privileged_accept_task::attach_observer(privileged_connection* observer) {
    std::lock_guard lg(_observer_mx);
    _connection_observer = observer;
}

void privileged_accept_task::accept_connections() {
    _acceptor.async_accept(
        [&](boost::system::error_code er, tcp::socket socket) {
            if (!er) {
                spdlog::info("Privileged client accepted");
                this->notify_client_accepted(socket);
                this->accept_connections();
            } else {
                spdlog::info("Accepting new client failed: {}", er.message());
            }
        });
}

void privileged_accept_task::notify_client_accepted(tcp::socket& socket) {
    std::lock_guard lg(_observer_mx);
    if (_connection_observer) {
        _connection_observer->update_server_accepted(std::make_shared<privileged_server>(std::move(socket), 
                                                     _connection_observer));
    }
}
