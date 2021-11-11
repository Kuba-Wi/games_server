#pragma once

#include <atomic>
#include <condition_variable>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

#include <boost/asio.hpp>

using tcp = boost::asio::ip::tcp;

constexpr int8_t data_delimiter = std::numeric_limits<int8_t>::max();

class network;

class receive_worker {
public:
    receive_worker(std::shared_ptr<tcp::socket>& socket, network* observer);
    ~receive_worker();

    void start_receive_data();

private:
    void receive_data();
    void refresh_data_buffer(size_t bytes_with_delimiter);
    void add_to_received_queue(const std::vector<int8_t>& data, size_t size);
    void data_received_loop();

    void notify_data_received(const std::vector<int8_t>& data);
    void notify_disconnected(const std::string& message);

    std::vector<int8_t> _data_received;
    std::list<std::vector<int8_t>> _received_queue;
    std::mutex _received_queue_mx;
    std::condition_variable _received_queue_cv;
    std::thread _data_recived_loop_th;

    std::atomic<bool> _stop_worker{false};

    std::shared_ptr<tcp::socket> _socket_ptr;

    network* _network_observer;
};
