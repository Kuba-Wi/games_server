#include "server.h"
#include "servers.h"
#include "socket_option.h"

#include <spdlog/spdlog.h>

server::server(boost::asio::ip::tcp::socket&& socket, servers* servers) : 
               _socket(std::move(socket)),
               _servers_observer(servers),
               _send_task_ptr(std::make_shared<send_task>(_socket)) {

    set_socket_no_delay_option(_socket);
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

void server::notify_data_received() const {
    _servers_observer->update_data_received(_data_buffer); 
}

void server::notify_server_disconnected() {
    _servers_observer->update_disconnected(this->shared_from_this());
}
