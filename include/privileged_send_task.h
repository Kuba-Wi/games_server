#pragma once

#include <atomic>
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "send_task.h"
//TODO: template send_task 

// constexpr int8_t data_delimiter = std::numeric_limits<int8_t>::max();


using tcp = boost::asio::ip::tcp;
using ssl_socket = boost::asio::ssl::stream<tcp::socket>;

using send_type = std::vector<int8_t>;
using send_iterator = std::list<send_type>::iterator;

class privileged_send_task : public std::enable_shared_from_this<privileged_send_task> {
public:
    privileged_send_task(const std::shared_ptr<ssl_socket>& sock);
    ~privileged_send_task();
    void send_data(const send_type& data);
    void send_large_number(size_t number, int8_t signal);
    void stop_task();

private:
    void start_task();
    void erase_el_from_queue(const send_iterator& it);
    void execute_send(std::unique_lock<std::mutex>&& ul_send_mx);
    void send_loop();

    std::shared_ptr<ssl_socket> _socket;
    std::atomic<bool> _send_executing{false};
    std::atomic<bool> _end_task{false};

    std::list<send_type> _send_queue;
    const size_t _queue_max_size = 1000;

    std::mutex _send_mx;
    std::condition_variable _send_data_cv;
    std::thread _send_loop_th;
};
