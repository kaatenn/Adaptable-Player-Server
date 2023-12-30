#include "api/KcpServer.h"

#include "thread"
#include <string>
#include "iostream"
#include "api/TcpServer.h"
#include "example/ExampleApplicationProtocol/EP.h"

using namespace kaatenn;

int main() {
    try {
        // kcp asio init
        EP ep_kcp;
        unsigned short port = 12345;
        KCPServer server(port, &ep_kcp);

        // tcp asio init
        EP ep_tcp;
        unsigned short tcp_port = 12346;
        TCPServer tcp_server(tcp_port, &ep_tcp);

        server.run_server();
        tcp_server.run_server();

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}