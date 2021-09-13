#pragma once

#include <boost/asio.hpp>

void set_socket_no_delay_option(boost::asio::ip::tcp::socket& socket);
