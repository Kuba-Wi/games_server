#include "connection_initializers.h"

void set_socket_no_delay_option(boost::asio::ip::tcp::socket&) {}
void use_ssl_cert_and_key(boost::asio::ssl::context&) {}
