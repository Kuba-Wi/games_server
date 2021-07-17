#pragma once

#include <list>
#include <memory>

#include "server.h"

class servers {
public:
    servers();
    ~servers();
    void accept_new_clients();
    template <typename T>
    void send_data(const std::pair<std::vector<T>, size_t>& data);
    uint8_t get_data_received();

private:
    std::list<std::unique_ptr<server>> _server_list;

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::endpoint _server_endpoint;
    boost::asio::ip::tcp::acceptor _acceptor;
    boost::asio::ip::tcp::socket _tmp_socket;

    std::thread _io_context_th;
    std::mutex _list_mx;
    std::atomic<uint8_t> _data_received{0};
};

template <typename T>
void servers::send_data(const std::pair<std::vector<T>, size_t>& data) {
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
