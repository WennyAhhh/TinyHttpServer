#pragma once
#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include "channel.h"

typedef std::function<void(int, const sockaddr_in &)> NewConnectionCallBack;

class Acceptor
{
public:
    Acceptor(EventLoop *loop, const sockaddr_in &listen_addr, bool reuseport);
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
    Channel accept_channel_;
    NewConnectionCallBack new_conn_callback_;
    bool listening_;
    int fd_;
};

#endif