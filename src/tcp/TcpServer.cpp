//
// Created by 86137 on 2023/12/28.
//

#include "../../include/TcpServer.h"
#include "response/UrlMap.hpp"
#include <iostream>
#include <Connection.hpp>

TCPServer::TCPServer(asio::io_context &io_context, unsigned short port) :io_context(io_context), socket(io_context),
remote_endpoint(asio::ip::tcp::v4(), port), acceptor(io_context) {
    acceptor.open(remote_endpoint.protocol());
    acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(remote_endpoint);
    asio::error_code ec;
    acceptor.listen(asio::socket_base::max_listen_connections, ec);
    if (ec) {
        std::cerr << "acceptor listen error: " << ec.message() << std::endl;
        exit(1);
    } else {
        std::cout << "acceptor listen on port: " << port << std::endl;
    }
    start_receive();
}

void TCPServer::start_receive() {
    acceptor.async_accept(socket, [this](const asio::error_code& ec) {
        if (!ec) {
            std::make_shared<TCPConnection>(std::move(socket))->start(); // shaking hands
        } else {
            std::cerr << "accept error: " << ec.message() << std::endl;
        }
    });
}


void TCPConnection::start() {
    do_read();
}

void TCPConnection::do_read() {
    auto self(shared_from_this());
    receive_buffer.fill(0);
    socket.async_read_some(asio::buffer(receive_buffer), [this, self](std::error_code ec, std::size_t byte_recv) {
        if (!ec) {
            on_receive(receive_buffer.data(), byte_recv);
        } else {
            std::cerr << "read error: " << ec.message() << std::endl;
        }
    });
}

void TCPConnection::on_receive(char* data, size_t recvd) {
    ending_assert_string.append(data, recvd);
    Connection connection = Connection::from_json(ending_assert_string);
    if (connection.get_url() != "error") {
        string url = connection.get_url();
        if (url_map.count(url) != 0) {
            ResBuffer res_buffer = url_map[url](connection);
            int segment_size = 1024;
            for (const auto& buffer : res_buffer) {
                for (int i = 0; i < buffer.size(); i += segment_size) {
                    int size = std::min(segment_size, (int)buffer.size() - i);
                    this->send(buffer.data() + i, size);
                }
            }
        }
        ending_assert_string.clear();
    }
    do_read();
}

void TCPConnection::send(const char *data, size_t length) {
    auto self(shared_from_this());
    socket.async_write_some(asio::buffer(data, length), [this, self](std::error_code ec, std::size_t bytes_sent) {
        if (ec) {
            std::cout << "send error: " << ec.message() << std::endl;
        }
        if (!ec && bytes_sent > 0) {
            std::cout << "send data: " << bytes_sent << std::endl;
        }
    });
}
