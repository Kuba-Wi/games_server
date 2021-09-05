#pragma once

#include <boost/asio.hpp>

#include <atomic>
#include <thread>

#include "test_iface.h"

constexpr size_t timeout_seconds = 20;

class servers;

class timeout_task {
public:
    timeout_task();
    TEST_IFACE ~timeout_task();
    TEST_IFACE void attach_observer(servers* observer);
    TEST_IFACE void reset_deadline();

private:
    void check_timer();
    void notify_timeout();

    std::atomic<bool> _timer_stopped{false};
    boost::asio::io_context _io_context;
    boost::asio::deadline_timer _timer;
    std::thread _io_context_th;

    servers* _servers_observer = nullptr;
    std::mutex _observer_mx;
};
