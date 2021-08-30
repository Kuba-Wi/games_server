#include "server.h"

#include <spdlog/spdlog.h>

server::server(boost::asio::ip::tcp::socket&& socket, Iservers* servers) : 
               _socket(std::move(socket)),
               _servers_observer(servers),
               _send_task_ptr(std::make_shared<send_task>(_socket)) {

    boost::system::error_code er;
    _socket.set_option(boost::asio::ip::tcp::no_delay(true), er);
    if (er) {
        spdlog::error("Set socket option (no delay): {}", er.message());
    }
}

void server::receive_data() {
    boost::asio::async_read(
        _socket,
        boost::asio::buffer(&_data_buffer, sizeof(_data_buffer)),
        [ptr = this->shared_from_this()](const boost::system::error_code& er, size_t) {
            if (!er) {
                ptr->notify_data_received();
                ptr->receive_data();
            } else {
                ptr->notify_server_disconnected();
                spdlog::info("Receive: {}. Client disconnected", er.message());
            }
        });
}

void server::send_data(const send_type& data) {
    _send_task_ptr->send_data(data);
}
