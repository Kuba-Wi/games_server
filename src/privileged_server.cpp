#include "privileged_server.h"

#include <spdlog/spdlog.h>

#include "privileged_connection.h"

privileged_server::privileged_server(const std::shared_ptr<ssl_socket>& socket, privileged_connection* observer) :
               _socket_ptr(socket),
               _send_task_ptr(std::make_shared<send_task<ssl_socket>>(_socket_ptr)),
               _connection_observer(observer) {}

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

void privileged_server::send_large_number(size_t number, int8_t signal) {
    _send_task_ptr->send_large_number(number, signal);
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