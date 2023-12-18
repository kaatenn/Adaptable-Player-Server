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


        std::thread server_thread([&]() {
            while (true) {
                server.update();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });

        server_thread.join();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}