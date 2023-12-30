#include "api/KcpServer.h"

#include "thread"
#include <string>
#include "iostream"
#include "api/TcpServer.h"

using namespace kaatenn;

int main() {
    try {
        // kcp asio init
        unsigned short port = 12345;
        KCPServer server(port);

        // tcp asio init
        unsigned short tcp_port = 12346;
        TCPServer tcp_server(tcp_port);

        server.run_server();
        tcp_server.run_server();

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}