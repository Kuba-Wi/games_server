#include "privileged_accept_task.h"

#include <spdlog/spdlog.h>

#include "privileged_connection.h"
#include "program_options.h"

privileged_accept_task::privileged_accept_task() : _server_endpoint(boost::asio::ip::tcp::v4(), privileged_port_number), 
                                                   _ssl_context(boost::asio::ssl::context::sslv23),
                                                   _acceptor(_io_context, _server_endpoint.protocol()) {
    try {
        _acceptor.bind(_server_endpoint);
        _acceptor.listen(boost::asio::socket_base::max_listen_connections);

        _ssl_context.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_tlsv1_3);
        _ssl_context.use_certificate_file("server_cert.pem", boost::asio::ssl::context::pem);
        _ssl_context.use_private_key_file("server_private_key.pem", boost::asio::ssl::context::pem);

    } catch (const boost::system::system_error& er) {
        spdlog::error("Server initialisation failed: {}. \nTerminate.", er.what());
        std::abort();
    } catch(...) {
        spdlog::error("Server initialisation failed with unexpected error. \nTerminate.");
        std::abort();
    }

    _io_context_th = std::thread{[&](){
        using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
        work_guard_type work_guard(_io_context.get_executor());
        _io_context.run();
    }};
}

privileged_accept_task::~privileged_accept_task() {
    _io_context.stop();
    _io_context_th.join();
}

void privileged_accept_task::attach_observer(privileged_connection* observer) {
    std::lock_guard lg(_observer_mx);
    _connection_observer = observer;
}

void privileged_accept_task::do_handshake(const std::shared_ptr<ssl_socket>& socket) {
    if (get_program_options().trust_all) {
        socket->set_verify_mode(boost::asio::ssl::verify_none);
    } else {
        socket->set_verify_mode(boost::asio::ssl::verify_peer | boost::asio::ssl::verify_fail_if_no_peer_cert);
        socket->set_verify_callback([&](bool preverified, boost::asio::ssl::verify_context& ctx){
            return this->verify_client_certificate(preverified, ctx);
        });
    }

    socket->async_handshake(boost::asio::ssl::stream_base::server,
        [&, socket](const boost::system::error_code& er) {
            if (!er) {
                spdlog::info("Privileged client accepted");
                this->notify_client_accepted(socket);
            } else {
                spdlog::info("Handshake failed: {}", er.message());
            }
        });
}

bool privileged_accept_task::verify_client_certificate(bool preverified, boost::asio::ssl::verify_context& ctx) {
    if (preverified) {
        return true;
    }

    X509* received_cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());

    FILE* client_cert_file = fopen("player_cert.pem", "r");
    if (client_cert_file == NULL) {
        spdlog::error("Handshake: Could not open the file with model client's certyficate");
        return false;
    }
    X509* model_client_cert = PEM_read_X509(client_cert_file, NULL, NULL, NULL);
    fclose(client_cert_file);

    int result = X509_cmp(model_client_cert, received_cert); //returns 0 if certificates are identical
    return result == 0;
  }

void privileged_accept_task::accept_connections() {
    _acceptor.async_accept(
        [&](boost::system::error_code er, tcp::socket socket) {
            if (!er) {
                this->do_handshake(std::make_shared<ssl_socket>(std::move(socket), _ssl_context));
                this->accept_connections();
            } else {
                spdlog::info("Accepting new client failed: {}", er.message());
            }
        });
}

void privileged_accept_task::notify_client_accepted(const std::shared_ptr<ssl_socket>& socket) {
    std::lock_guard lg(_observer_mx);
    if (_connection_observer) {
        _connection_observer->update_server_accepted(std::make_shared<privileged_server>(socket, 
                                                     _connection_observer));
    }
}
