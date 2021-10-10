#pragma once

#include <atomic>
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

#include <boost/asio.hpp>

constexpr int8_t data_delimiter = std::numeric_limits<int8_t>::max();

using send_type = std::vector<int8_t>;
using send_iterator = std::list<send_type>::iterator;

template <typename socket_t>
class send_task : public std::enable_shared_from_this<send_task<socket_t>> {
public:
    send_task(const std::shared_ptr<socket_t>& sock);
    ~send_task();
    void send_data(const send_type& data);
    void send_large_number(size_t number, int8_t signal);
    void stop_task();

private:
    void start_task();
    void erase_el_from_queue(const send_iterator& it);
    void execute_send(std::unique_lock<std::mutex>&& ul_send_mx);
    void send_loop();

    std::shared_ptr<socket_t> _socket;
    std::atomic<bool> _send_executing{false};
    std::atomic<bool> _end_task{false};

    std::list<send_type> _send_queue;
    const size_t _queue_max_size = 1000;

    std::mutex _send_mx;
    std::condition_variable _send_data_cv;
    std::thread _send_loop_th;
};

template <typename socket_t>
send_task<socket_t>::send_task(const std::shared_ptr<socket_t>& sock) : _socket(sock) {
    this->start_task();
}

template <typename socket_t>
send_task<socket_t>::~send_task() {
    this->stop_task();
}

template <typename socket_t>
void send_task<socket_t>::start_task() {
    _send_loop_th = std::thread{[&](){
        this->send_loop();
    }};
}

template <typename socket_t>
void send_task<socket_t>::stop_task() {
    std::unique_lock ul(_send_mx);
    _end_task = true;
    ul.unlock();
    _send_data_cv.notify_all();
    if (_send_loop_th.joinable()) {
        _send_loop_th.join();
    }
}

template <typename socket_t>
void send_task<socket_t>::send_data(const send_type& data) {
    std::unique_lock ul(_send_mx);
    if (_send_queue.size() >= _queue_max_size) {
        return;
    }
    _send_queue.push_back(data);
    _send_queue.back().push_back(data_delimiter);
    ul.unlock();
    _send_data_cv.notify_all();
}

template <typename socket_t>
void send_task<socket_t>::send_large_number(size_t number, int8_t signal) {
    send_type data_to_send{signal};

    while(number >= data_delimiter) {
        data_to_send.emplace_back(data_delimiter - 1);
        number -= (data_delimiter - 1);
    }
    data_to_send.emplace_back(number);
    this->send_data(data_to_send);
}

template <typename socket_t>
void send_task<socket_t>::execute_send(std::unique_lock<std::mutex>&& ul_send_mx) {
    _send_executing = true;
    boost::asio::mutable_buffer buf(_send_queue.front().data(), _send_queue.front().size() * sizeof(send_type::value_type));
    auto it = _send_queue.begin();
    ul_send_mx.unlock();

    boost::asio::async_write(*_socket, buf, 
        [it, ptr = this->shared_from_this()](const boost::system::error_code&, size_t){
            ptr->erase_el_from_queue(it);
            ptr->_send_executing = false;
            ptr->_send_data_cv.notify_all();
        });
}

template <typename socket_t>
void send_task<socket_t>::send_loop() {
    while (!_end_task) {
        std::unique_lock ul_send_mx(_send_mx);
        _send_data_cv.wait(ul_send_mx, [&](){
            return (_send_queue.size() > 0 && !_send_executing) || _end_task;
        });
        if (_end_task) {
            return;
        }
        this->execute_send(std::move(ul_send_mx));
    }
}

template <typename socket_t>
void send_task<socket_t>::erase_el_from_queue(const send_iterator& it) {
    std::lock_guard lg(_send_mx);
    _send_queue.erase(it);
}
