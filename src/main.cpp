#include "../include/KcpServer.h"

#include "thread"
#include <string>
#include "iostream"
#include "../include/TcpServer.h"

int main() {
    try {
        // kcp asio init
        asio::io_context io_context;

        unsigned short port = 12345;
        KCPServer server(io_context, port);

        server.start_receive();

        // tcp asio init
        asio::io_context tcp_io_context;
        unsigned short tcp_port = 12346;
        TCPServer tcp_server(tcp_io_context, tcp_port);

        std::thread asio_thread([&io_context]() { io_context.run(); });
        std::thread tcp_asio_thread([&tcp_io_context]() { tcp_io_context.run(); });

        asio_thread.join();
        tcp_asio_thread.join();

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}