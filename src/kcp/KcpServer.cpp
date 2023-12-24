//
// Created by 86137 on 2023/12/17.
//

#include "KcpServer.h"
#include "fstream"
#include "string"
#include "response/UrlMap.hpp"

using std::string;

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
    ikcp_input(kcp, data, length);
    int recv_size = ikcp_recv(kcp, receive_buffer.data(), receive_buffer.size());
    socket.connect(remote_endpoint);
    start_receive();
    if (strcmp(receive_buffer.data(), "hello") != 0) {
        std::cout << "receive data: " << receive_buffer.data() << std::endl;
        return;
    }
    // Response with a file
    string url = receive_buffer.data();
    std::ifstream file("./11582.mp3", std::ios::binary);
    if (!file.is_open()) {
        std::cout << "file not found" << std::endl;
        return;
    }
    std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});

    int segment_size = 900; // leave some space for kcp header and '\0'
    for (int i = 0; i < buffer.size(); i += segment_size) {
        int size = std::min(segment_size, (int)buffer.size() - i);
        this->send(buffer.data() + i, size);
    }
    this->send("end", 4);

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

KCPServer::KCPServer(asio::io_context &io_context, unsigned short port)
        : io_context(io_context), socket(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)) {
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
}

void KCPServer::send(const char *data, size_t length) {
    ikcp_send(kcp, data, length);
    // 存储或更新客户端的endpoint
}
