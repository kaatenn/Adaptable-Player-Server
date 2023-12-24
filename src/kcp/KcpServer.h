//
// Created by 86137 on 2023/12/17.
//

#ifndef ADAPTABLE_SERVER_KCPSERVER_H
#define ADAPTABLE_SERVER_KCPSERVER_H

#include <asio.hpp>
#include "ikcp.h"
#include "string"
#include "iostream"

class KCPServer {
public:
    KCPServer(asio::io_context& io_context, unsigned short port);

    ~KCPServer() {
        ikcp_release(kcp);
    }

    void start_receive();
    void on_receive(const char* data, size_t length);

    void update();

    void send(const char *data, size_t length);

private:
    asio::io_context& io_context;
    asio::ip::udp::socket socket;
    asio::ip::udp::endpoint remote_endpoint;
    asio::steady_timer* timer;
    std::array<char, 1024> receive_buffer;
    std::array<char, 1024> file_buffer;
    ikcpcb *kcp;

    static int udp_output(const char *buf, int len, ikcpcb *kcp, void *user);

    static inline uint32_t iclock()
    {
        using namespace std::chrono;
        return static_cast<uint32_t>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    }
};


#endif //ADAPTABLE_SERVER_KCPSERVER_H
