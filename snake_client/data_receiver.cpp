#include "data_receiver.h"

data_receiver::data_receiver(boost::asio::io_context& io) : _socket(io), _data_received(buffer_size) {
    _data_received_loop_th = std::thread{[&](){
        this->data_received_loop();
    }};
}

data_receiver::~data_receiver() {
    _stop_receiver = true;
    _received_queue_cv.notify_all();
    _data_received_loop_th.join();
}

void data_receiver::start_receiver(size_t receiver_port) {
    this->prepare_receive(receiver_port);
    this->receive_data();
}

void data_receiver::prepare_receive(size_t receiver_port) {
    boost::system::error_code er;
    if (_socket.is_open()) {
        _socket.close(er);
    }
    _socket.open(udp::v4(), er);
    _socket.bind(udp::endpoint(udp::v4(), receiver_port), er);
    if (er) {
        std::terminate();
    }
}

void data_receiver::receive_data() {
    if (_stop_receiver) {
        return;
    } 
    _socket.async_receive_from(
        boost::asio::mutable_buffer(_data_received.data(), _data_received.size()), _server_endpoint,
        [&](const boost::system::error_code& er, size_t bytes_received) {
                if (!er) {
                    this->add_to_received_queue(bytes_received);
                    this->receive_data();
                }
        });
}

void data_receiver::add_to_received_queue(size_t size) {
    std::unique_lock ul(_received_queue_mx);
    _received_queue.emplace_back(_data_received.begin(), _data_received.begin() + size);
    ul.unlock();
    _received_queue_cv.notify_all();
}

void data_receiver::notify_update(const std::vector<int8_t>& data) const {
    if (_network_observer) {
        _network_observer->update_data_received(data); 
    }
}

void data_receiver::data_received_loop() {
    while (!_stop_receiver) {
        std::unique_lock ul(_received_queue_mx);
        _received_queue_cv.wait(ul, [&](){
            return _received_queue.size() > 0 || _stop_receiver;
        });
        if (_stop_receiver) {
            return;
        }

        auto end = _received_queue.end();
        ul.unlock();

        for (auto it = _received_queue.begin(); it != end; ++it) {
            this->notify_update(*it);
        }
        std::lock_guard lg(_received_queue_mx);
        _received_queue.erase(_received_queue.begin(), end);
    }
}
