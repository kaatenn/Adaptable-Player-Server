//
// Created by 86137 on 2023/12/17.
//

#include "api/KcpServer.h"
#include "example/ExampleApplicationProtocol/UrlMap.hpp"
#include "fstream"
#include "string"
#include "example/Connection.hpp"
#include "detail/error_handler.h"


using std::string;
using namespace kaatenn;

KCPServer::KCPServer(unsigned short port)
        : socket(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)) {
    check_port(port);

    // init kcp
    kcp = ikcp_create(0x11223344, (void*)this);
    ikcp_setoutput(kcp, &KCPServer::udp_output);

    // add timer
    this->timer = new asio::steady_timer(io_context, asio::chrono::milliseconds(10));
    timer->async_wait([this](std::error_code ec) {
        if (!ec) {
            this->update();
        }
    });
    start_receive();

    asio_thread = std::thread([this]() { io_context.run(); });
}

void KCPServer::run_server() {
    asio_thread.join();
}

void KCPServer::start_receive() {
    socket.async_receive_from(
            asio::buffer(receive_buffer), remote_endpoint,
            [this](std::error_code ec, std::size_t bytes_recvd) {
                if (!ec && bytes_recvd > 0) {
                    on_receive(receive_buffer.data(), bytes_recvd);
                }
            }
    );
}

void KCPServer::on_receive(const char *data, size_t length) {
    ikcp_input(kcp, data, length); // manage a receiving package
    int recv_size;
    string ending_asserting_buffer;
    while((recv_size = ikcp_recv(kcp, file_buffer.data(), file_buffer.size())) > 0) {
        // When we could successfully receive a legal json, we assert the ending of the response.
        ending_asserting_buffer.append(file_buffer.data(), recv_size);
        std::fill(file_buffer.begin(), file_buffer.end(), 0);
        Connection response = Connection::from_json(ending_asserting_buffer);
        if (response.get_url() != "error") {
            break;
        }
    }
    socket.connect(remote_endpoint);
    start_receive();

    Connection connection = Connection::from_json(ending_asserting_buffer);
    // If receive an ACK, then return
    if (url_map.count(connection.get_url()) == 0) {
        std::cout << "receive ack or 404 not found" << std::endl;
        return;
    }
    ResBuffer res_buffer = url_map[connection.get_url()](connection);

    int segment_size = 900; // leave some space for kcp header and '\0'
    for (const auto& buffer : res_buffer) {
        // We should send different segments for every buffer
        for (int i = 0; i < buffer.size(); i += segment_size) {
            int size = std::min(segment_size, (int)buffer.size() - i);
            this->send(buffer.data() + i, size);
        }
    }

    if (recv_size > 0) {
        std::cout << "receive data: " << receive_buffer.data() << std::endl;
    } else {
        std::cout << "receive ack" << std::endl;
    }
}

void KCPServer::update() {
    ikcp_update(kcp, iclock());

    // refresh timer
    timer->expires_after(asio::chrono::milliseconds(10));
    timer->async_wait([this](std::error_code ec) {
        if (!ec) {
            this->update();
        }
    });
}

int KCPServer::udp_output(const char *buf, int len, ikcpcb *kcp, void *user) {
    auto *server = (KCPServer*)user;
    server->socket.async_send(asio::buffer(buf, len), [](std::error_code ec, std::size_t bytes_sent) {
        if (ec) {
            std::cout << "send error: " << ec.message() << std::endl;
        }
        if (!ec && bytes_sent > 0) {
            std::cout << "send data: " << bytes_sent << std::endl;
        }
    });
    return 0;
}

void KCPServer::send(const char *data, size_t length) {
    ikcp_send(kcp, data, length);
}
