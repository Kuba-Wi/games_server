#pragma once
#include <boost/asio.hpp>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

class client_connection {
public:
    client_connection();
    ~client_connection();

    void send_data(uint8_t data);
    bool check_index_present(uint8_t x, uint8_t y) const;
    void refresh_client();
private:
    void receive_data();
    
    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::endpoint _server_endpoint;
    boost::asio::ip::tcp::socket _socket;

    std::thread _io_context_thread;
    mutable std::mutex _data_mutex;
    mutable std::mutex _client_data_mutex;
    std::mutex _send_mutex;
    std::condition_variable _keep_rec_cv;

    uint8_t _data_to_send;
    size_t _bytes_received{0};
    std::atomic<bool> _keep_receiving{true};

    std::vector<std::pair<uint8_t, uint8_t>> _data_received;
    std::vector<std::pair<uint8_t, uint8_t>> _client_data;
};
