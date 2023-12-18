//
// Created by 86137 on 2023/12/17.
//

#include "KcpServer.h"

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
    send(receive_buffer.data(), recv_size, remote_endpoint);
    if (recv_size > 0) {
        std::cout << "receive data: " << receive_buffer.data() << std::endl;
    }
}
