#include "privileged_server.h"

#include <spdlog/spdlog.h>

#include "privileged_connection.h"
#include "socket_option.h"

privileged_server::privileged_server(tcp::socket&& socket, privileged_connection* observer) :
               _socket_ptr(std::make_shared<tcp::socket>(std::move(socket))),
               _send_task_ptr(std::make_shared<send_task>(_socket_ptr)),
               _connection_observer(observer) {

    set_socket_no_delay_option(*_socket_ptr);
}

void privileged_server::receive_data() {
    async_read_until(*_socket_ptr,
        boost::asio::dynamic_buffer(_data_buffer), data_delimiter,
        [ptr = this->shared_from_this()](const boost::system::error_code& er, size_t bytes_with_delimiter) {
            if (!er) {
                ptr->notify_data_received(bytes_with_delimiter - 1);
                ptr->refresh_data_buffer(bytes_with_delimiter);
                ptr->receive_data();
            } else {
                ptr->notify_server_disconnected();
                spdlog::info("Receive: {}. Privileged client disconnected", er.message());
            }
        });
}

void privileged_server::send_data(const std::vector<int8_t>& data) {
    _send_task_ptr->send_data(data);
}

void privileged_server::refresh_data_buffer(size_t bytes_with_delimiter) {
    _data_buffer.erase(_data_buffer.begin(), _data_buffer.begin() + bytes_with_delimiter);
}

void privileged_server::notify_data_received(size_t bytes) const {
    _connection_observer->update_data_received(std::vector(_data_buffer.begin(), _data_buffer.begin() + bytes));
}

void privileged_server::notify_server_disconnected() {
    _connection_observer->update_disconnected();
}
