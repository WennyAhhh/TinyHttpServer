#pragma once
#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <unistd.h>
#include <fcntl.h>

#include "inetaddress.h"
class SocketServer
{
public:
    static constexpr int MAXBACKLOG = 5;
    explicit SocketServer(int sockfd) : sockfd_(sockfd)
    {
    }
    ~SocketServer()
    {
        if (::close(sockfd_) < 0)
        {
            LOG_WARN("socket close");
        }
    }
    SocketServer(SocketServer &) = delete;
    SocketServer &operator=(SocketServer &) = delete;

    int fd() const { return sockfd_; }
    void set_reuse_addr(bool);
    void set_reuse_port(bool);

    void bind(const InetAddress &inet_addr);
    void listen();
    int accept(InetAddress &peeraddr);
    int accept();

private:
    const int sockfd_;
};

#endif