#include "send_task.h"

send_task::send_task(const std::shared_ptr<tcp::socket>& sock) : _socket(sock) {
    _send_loop_th = std::thread{[&](){
        this->send_loop();
    }};
}

send_task::~send_task() {
    _end_task = true;
    _send_data_cv.notify_all();
    _send_loop_th.join();
}

void send_task::send_data(const send_type& data) {
    std::unique_lock ul(_send_mx);
    if (_send_queue.size() >= _queue_max_size) {
        return;
    }
    _send_queue.push_back(data);
    _send_queue.back().push_back(data_delimiter);
    ul.unlock();
    _send_data_cv.notify_all();
}

void send_task::execute_send() {
    _send_executing = true;
    boost::asio::mutable_buffer buf(_send_queue.front().data(), _send_queue.front().size() * sizeof(send_type::value_type));
    auto it = _send_queue.begin();

    boost::asio::async_write(*_socket, buf, 
        [it, ptr = this->shared_from_this()](const boost::system::error_code&, size_t){
            ptr->erase_el_from_queue(it);
            ptr->_send_executing = false;
            ptr->_send_data_cv.notify_all();
        });
}

void send_task::send_loop() {
    while (!_end_task) {
        std::unique_lock ul(_send_mx);
        _send_data_cv.wait(ul, [&](){
            return (_send_queue.size() > 0 && !_send_executing) || _end_task;
        });
        if (_end_task) {
            return;
        }
        this->execute_send();
    }
}

void send_task::erase_el_from_queue(const send_iterator& it) {
    std::lock_guard lg(_send_mx);
    _send_queue.erase(it);
}
