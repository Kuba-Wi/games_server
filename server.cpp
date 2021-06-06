#include "server.h"
#include <iostream>
#include <vector>

server::server() : _socket(_io_context) {
        boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::udp::v4(), 30000);
        _socket.open(endpoint.protocol());
        boost::system::error_code er;
        _socket.bind(endpoint, er);
        if (er) {
            std::cerr << er.message() << "\n";
        }
    }

void server::receive_data() {
    boost::system::error_code er;
    std::vector<uint8_t> recv_buf(100);
    try {
        _socket.receive_from(boost::asio::buffer(recv_buf, 100), _client_endpoint);
    } catch(boost::system::error_code& er) {
        std::cerr << er.message() << std::endl;
    } catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    } catch(...) {
        std::cerr << "Unrecognized exception in receive_data function\n";
    }
}
