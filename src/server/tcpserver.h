#pragma once
#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcpconnection.h"

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer
{
public:
    typedef std::function<void(EventLoop *)> ThreadInitCallback;
    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

    TcpServer(EventLoop *loop,
              const InetAddress &listen_address,
              const std::string &name_arg,
              bool option = true);
    ~TcpServer();

    const std::string &get_ip_port() const { return ip_port_; }
    const std::string &name() const { return name_; }

    void start();

    void set_thread_num(int num_threads);
    void set_thread_init_cb_(const ThreadInitCallback &cb) { thread_init_cb_ = cb; }

    void set_connection_cb(const ConnectionCallback &cb) { connection_cb_ = cb; }
    void set_message_cb(const MessageCallback &cb) { message_cb_ = cb; }
    void set_write_complete_cb(const WriteCompleteCallback &cb) { write_complete_cb_ = cb; }

private:
    void new_connection(int sockfd, const InetAddress &peer_address);

    void remove_connection(const TcpConnectionPtr &conn);

    void remove_connection_in_loop(const TcpConnectionPtr &conn);

    EventLoop *loop_;
    const std::string ip_port_;
    const std::string name_;
    std::unique_ptr<Acceptor> acceptor_;
    std::unique_ptr<EventLoopThreadPool> thread_pool_;

    ConnectionCallback connection_cb_;
    MessageCallback message_cb_;
    WriteCompleteCallback write_complete_cb_;
    ThreadInitCallback thread_init_cb_;

    int next_id;
    ConnectionMap connection_;
};

#endif