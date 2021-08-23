#pragma once

#include <boost/asio.hpp>

#include <atomic>
#include <condition_variable>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

using udp = boost::asio::ip::udp;

constexpr size_t buffer_size = 1000;

class Inetwork {
public:
    virtual ~Inetwork() = default;
    virtual void update_data_received(const std::vector<int8_t>& data) = 0;
};

class data_receiver {
public:
    data_receiver(boost::asio::io_context& io);
    ~data_receiver();
    void attach_observer(Inetwork* observer) { _network_observer = observer; }
    void start_receiver(size_t receiver_port);

private:
    void receive_data();
    void add_to_received_queue(size_t size);
    void notify_update(const std::vector<int8_t>& data) const;
    void data_received_loop();

    udp::endpoint _server_endpoint;
    udp::socket _socket;

    std::vector<int8_t> _data_received;
    std::list<std::vector<int8_t>> _received_queue;
    std::mutex _received_queue_mx;
    std::condition_variable _received_queue_cv;
    std::thread _data_received_loop_th;
    
    std::atomic<bool> _stop_receiver{false};

    Inetwork* _network_observer = nullptr;
};
