#include "kcp/KcpServer.h"

#include "thread"
#include <string>
#include "iostream"

int main() {
    try {
        asio::io_context io_context;

        unsigned short port = 12345;
        KCPServer server(io_context, port);

        server.start_receive();
        io_context.run();

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}