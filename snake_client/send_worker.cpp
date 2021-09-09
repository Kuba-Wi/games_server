#include "send_worker.h"

send_worker::send_worker(tcp::socket& socket) : _socket(socket) {
    _send_loop_th = std::thread{[&](){
        this->send_loop();
    }};
}

send_worker::~send_worker() {
    _stop_worker = true;
    _send_data_cv.notify_all();
    _send_loop_th.join();
}

void send_worker::send_data(uint8_t data) {
    std::unique_lock ul(_send_queue_mx);
    _send_queue.push_back(data);
    ul.unlock();
    _send_data_cv.notify_all();
}

void send_worker::send_loop() {
    while (!_stop_worker) {
        std::unique_lock ul(_send_queue_mx);
        _send_data_cv.wait(ul, [&](){
            return (_send_queue.size() > 0 && !_send_executing) || _stop_worker;
        });
        if (_stop_worker) {
            return;
        }
        this->execute_send();
    }
}

void send_worker::execute_send() {
    _send_executing = true;
    boost::asio::mutable_buffer buf(&_send_queue.front(), sizeof(uint8_t));
    auto it = _send_queue.begin();

    boost::asio::async_write(_socket, buf, 
        [&, it](const boost::system::error_code&, size_t){
            this->erase_from_send_queue(it);
            _send_executing = false;
            _send_data_cv.notify_all();
        });
}

void send_worker::erase_from_send_queue(const std::list<uint8_t>::iterator& it) {
    std::lock_guard lg(_send_queue_mx);
    _send_queue.erase(it);
}
