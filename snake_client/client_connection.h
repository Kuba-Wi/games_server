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
    void copy_data();
private:
    void receive_data();
    
    boost::asio::io_context _io_context;
    boost::asio::ip::udp::endpoint _client_endpoint;
    boost::asio::ip::udp::endpoint _server_endpoint;
    boost::asio::ip::udp::socket _socket;

    std::thread _io_context_thread;
    mutable std::mutex _data_mutex;
    mutable std::mutex _data_cp_mutex;
    std::atomic<size_t> _bytes_received{0};
    mutable std::atomic<bool> _keep_receiving{true};
    mutable std::condition_variable _keep_rec_cv;

    std::vector<std::pair<uint8_t, uint8_t>> _data_received;
    std::vector<std::pair<uint8_t, uint8_t>> _data_copy;
};
