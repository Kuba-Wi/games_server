#include "socket_option.h"

void set_socket_no_delay_option(boost::asio::ip::tcp::socket& socket) {
    boost::system::error_code er;
    socket.set_option(boost::asio::ip::tcp::no_delay(true), er);
}
