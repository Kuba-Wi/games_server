#include "receive_worker.h"
#include "network.h"

receive_worker::receive_worker(tcp::socket& socket, network* observer) : _socket(socket), _network_observer(observer) {
    _data_recived_loop_th = std::thread{[&](){
        this->data_received_loop();
    }};
}

receive_worker::~receive_worker() {
    _stop_worker = true;
    _received_queue_cv.notify_all();
    _data_recived_loop_th.join();
}

void receive_worker::receive_data() {
    async_read_until(_socket,
        boost::asio::dynamic_buffer(_data_received), data_delimiter,
        [&](const boost::system::error_code& er, size_t bytes_with_delimiter) {
            if (!er) {
                this->add_to_received_queue(_data_received, bytes_with_delimiter - 1);
                this->refresh_data_buffer(bytes_with_delimiter);
                this->receive_data();
            } else {
                this->notify_disconnected();
            }
        });
}

void receive_worker::refresh_data_buffer(size_t bytes_with_delimiter) {
    _data_received.erase(_data_received.begin(), _data_received.begin() + bytes_with_delimiter);
}

void receive_worker::add_to_received_queue(const std::vector<int8_t>& data, size_t size) {
    std::unique_lock ul(_received_queue_mx);
    _received_queue.emplace_back(data.begin(), data.begin() + size);
    ul.unlock();
    _received_queue_cv.notify_all();
}

void receive_worker::data_received_loop() {
    while (!_stop_worker) {
        std::unique_lock ul(_received_queue_mx);
        _received_queue_cv.wait(ul, [&](){
            return _received_queue.size() > 0 || _stop_worker;
        });
        if (_stop_worker) {
            return;
        }

        auto end = _received_queue.end();
        ul.unlock();

        for (auto it = _received_queue.begin(); it != end; ++it) {
            this->notify_data_received(*it);
        }
        std::lock_guard lg(_received_queue_mx);
        _received_queue.erase(_received_queue.begin(), end);
    }
}

void receive_worker::notify_data_received(const std::vector<int8_t>& received_data) {
    if (_network_observer) {
        _network_observer->update_data_received(received_data); 
    }
}

void receive_worker::notify_disconnected() {
    if (_network_observer) {
        _network_observer->update_disconnected();
    }
}
