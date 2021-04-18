#pragma once
#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include "channel.h"
#include "server/socketserver.h"

typedef std::function<void(int, const InetAddress &)> NewConnectionCallBack;

class EventLoop;

class Acceptor
{
public:
    Acceptor(EventLoop *loop, const InetAddress &listen_addr, bool reuseport);
    ~Acceptor();

    void set_conn_callback(const NewConnectionCallBack &cb)
    {
        new_conn_callback_ = cb;
    }
    void listen();
    bool listening() const
    {
        return listening_;
    }

private:
    void handle_read_();

    EventLoop *loop_;
    std::unique_ptr<SocketServer> accept_socket_;
    std::unique_ptr<Channel> accept_channel_;
    NewConnectionCallBack new_conn_callback_;
    bool listening_{false};
    int idle_fd_;
};

#endif