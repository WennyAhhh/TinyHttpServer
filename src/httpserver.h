#pragma once
#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "server/tcpconnection.h"
#include "server/tcpserver.h"
#include "server/entry.h"
#include "event/eventloop.h"
#include <boost/circular_buffer.hpp>
#include <vector>

typedef std::shared_ptr<Entry> EntryPtr;
typedef std::weak_ptr<Entry> WeakEntryPtr;
typedef std::vector<EntryPtr> Bucket;
typedef boost::circular_buffer<Bucket> WeakConnectionList;

class HttpServer
{
public:
    HttpServer(EventLoop *loop,
               const InetAddress &listen_address,
               const std::string &name_arg,
               bool option = false)
        : loop_(loop),
          tcpserver_(std::make_unique<TcpServer>(loop, listen_address, name_arg, option)),
          connection_list_(30)
    {
        //   connection_list_
        connection_list_.resize(30);
        printf("%d", connection_list_.size());
        tcpserver_->set_message_cb(std::bind(&HttpServer::message_cb, this, std::placeholders::_1, std::placeholders::_2));
        tcpserver_->set_connection_cb(std::bind(&HttpServer::connection_cb, this, std::placeholders::_1));
        tcpserver_->set_thread_init_cb_(std::bind(&HttpServer::init_cb, this, std::placeholders::_1));
    }

    void start()
    {
        tcpserver_->start();
        // repeat
        loop_->run_after(1, std::bind(&HttpServer::ontime, this));
        loop_->run_after(5, std::bind(&Log::loading, Log::instance()));
    }

    void ontime() { connection_list_.push_back(Bucket()); }
    void extend_time(const TcpConnectionPtr &conn);

    void connection_cb(const TcpConnectionPtr &conn);

    void message_cb(const TcpConnectionPtr &conn, std::shared_ptr<Buffer> &buff);

    void init_cb(EventLoop *loop);

private:
    std::unique_ptr<TcpServer> tcpserver_;
    EventLoop *loop_;
    WeakConnectionList connection_list_;
};

#endif
