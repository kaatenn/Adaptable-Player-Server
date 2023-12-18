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
    KCPServer(asio::io_context& io_context, unsigned short port)
            : io_context(io_context), socket(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)) {

        kcp = ikcp_create(0x11223344, (void*)this);
        ikcp_setoutput(kcp, &KCPServer::udp_output);
        // 设置KCP参数
    }

    ~KCPServer() {
        ikcp_release(kcp);
    }

    void start_receive();
    void on_receive(const char* data, size_t length);

    void update() {
        ikcp_update(kcp, iclock());
        // 其他处理，例如接收数据等
    }

    void send(const char *data, size_t length, const asio::ip::udp::endpoint& client_endpoint) {
        ikcp_send(kcp, data, length);
        // 存储或更新客户端的endpoint
    }

private:
    asio::io_context& io_context;
    asio::ip::udp::socket socket;
    asio::ip::udp::endpoint remote_endpoint;
    std::array<char, 1024> receive_buffer;
    ikcpcb *kcp;

    static int udp_output(const char *buf, int len, ikcpcb *kcp, void *user) {
        auto *server = (KCPServer*)user;
        server->socket.send_to(asio::buffer(buf, len), server->remote_endpoint);
        return 0;
    }

    static inline uint32_t iclock()
    {
        using namespace std::chrono;
        return static_cast<uint32_t>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    }
};


#endif //ADAPTABLE_SERVER_KCPSERVER_H
