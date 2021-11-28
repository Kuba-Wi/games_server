#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

void set_socket_no_delay_option(boost::asio::ip::tcp::socket& socket);
void use_ssl_cert_and_key(boost::asio::ssl::context& ssl_context);
