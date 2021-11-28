#include "connection_initializers.h"

#include <spdlog/spdlog.h>

void set_socket_no_delay_option(boost::asio::ip::tcp::socket& socket) {
    boost::system::error_code er;
    socket.set_option(boost::asio::ip::tcp::no_delay(true), er);
    if (er) {
        spdlog::error("Set socket option (no delay): {}", er.message());
    }
}

void use_ssl_cert_and_key(boost::asio::ssl::context& ssl_context) {
    ssl_context.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_tlsv1_3);
    ssl_context.use_certificate_file("server_cert.pem", boost::asio::ssl::context::pem);
    ssl_context.use_private_key_file("server_private_key.pem", boost::asio::ssl::context::pem);
}
