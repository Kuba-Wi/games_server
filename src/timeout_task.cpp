#include "servers.h"
#include "timeout_task.h"

timeout_task::timeout_task() : _timer(_io_context) {
    _io_context_th = std::thread{[&](){
        using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
        work_guard_type work_guard(_io_context.get_executor());
        _io_context.run();
    }};

    this->reset_deadline();
    _timer.async_wait([&](const boost::system::error_code&){
        this->check_timer();
    });
}

timeout_task::~timeout_task() {
    _timer_stopped = true;
    _timer.cancel();
    _io_context.stop();
    _io_context_th.join();
}

void timeout_task::attach_observer(servers* observer) {
    std::lock_guard lg(_observer_mx);
    _servers_observer = observer;
}

void timeout_task::reset_deadline() {
    boost::system::error_code er;
    _timer.expires_from_now(boost::posix_time::seconds(timeout_seconds), er);
}

void timeout_task::check_timer() {
    if(_timer_stopped) {
        return;
    }

    if (_timer.expires_at() <= boost::asio::deadline_timer::traits_type::now()) {
        this->notify_timeout();
        this->reset_deadline();
    }

    _timer.async_wait([&](const boost::system::error_code&){
        this->check_timer();
    });
}

void timeout_task::notify_timeout() {
    std::lock_guard lg(_observer_mx);
    if (_servers_observer) {
        _servers_observer->update_timeout();
    }
}
