#pragma once

#include <boost/asio.hpp>

#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

using tcp = boost::asio::ip::tcp;

class send_worker {
public:
    send_worker(tcp::socket& socket);
    ~send_worker();
    void send_data(uint8_t data);

private:
    void send_loop();
    void execute_send();
    void erase_from_send_queue(const std::list<uint8_t>::iterator& it);

    std::mutex _send_queue_mx;
    std::list<uint8_t> _send_queue;
    std::thread _send_loop_th;
    std::condition_variable _send_data_cv;
    std::atomic<bool> _send_executing{false};
    std::atomic<bool> _stop_worker{false};

    tcp::socket& _socket;
};
