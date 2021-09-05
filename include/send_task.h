#pragma once

#include <boost/asio.hpp>

#include <atomic>
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

constexpr int8_t data_delimiter = std::numeric_limits<int8_t>::max();

using tcp = boost::asio::ip::tcp;
using send_type = std::vector<int8_t>;
using send_iterator = std::list<send_type>::iterator;

class send_task : public std::enable_shared_from_this<send_task> {
public:
    send_task(tcp::socket& sock);
    ~send_task();
    void send_data(const send_type& data);

private:
    void erase_el_from_queue(const send_iterator& it);
    void execute_send();
    void send_loop();

    tcp::socket& _socket;
    std::atomic<bool> _send_executing{false};
    std::atomic<bool> _end_task{false};

    std::list<send_type> _send_queue;
    const size_t _queue_max_size = 1000;

    std::mutex _send_mx;
    std::condition_variable _send_data_cv;
    std::thread _send_loop_th;
};