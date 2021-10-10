#pragma once

#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "send_task.h"

using boost::asio::ip::tcp;
using ssl_socket = boost::asio::ssl::stream<tcp::socket>;

class privileged_connection;

class privileged_server : public std::enable_shared_from_this<privileged_server> {
public:
    privileged_server(const std::shared_ptr<ssl_socket>& socket, privileged_connection* observer);
    ~privileged_server() { _send_task_ptr->stop_task(); }
    void receive_data();
    void send_data(const std::vector<int8_t>& data);
    void send_large_number(size_t number, int8_t signal);

private:
    void refresh_data_buffer(size_t bytes_with_delimiter);
    void notify_data_received(size_t bytes) const;
    void notify_server_disconnected();

    std::shared_ptr<ssl_socket> _socket_ptr;
    std::shared_ptr<send_task<ssl_socket>> _send_task_ptr;

    std::vector<int8_t> _data_buffer;

    privileged_connection* _connection_observer;
};
