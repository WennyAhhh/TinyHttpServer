#pragma once
#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "server/tcpconnection.h"
#include "server/tcpserver.h"
#include "server/entry.h"
#include "event/eventloop.h"
#include "http/httpcontext.h"

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
               bool option = true)
        : loop_(loop),
          tcpserver_(std::make_unique<TcpServer>(loop, listen_address, name_arg, option)),
          connection_list_(90)
    {
        //   connection_list_
        connection_list_.resize(90);
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

    void message_cb(const TcpConnectionPtr &conn, Buffer *buff);

    void init_cb(EventLoop *loop);

    void process(const TcpConnectionPtr &conn, Buffer *readBuff);

    void set_dir(char *dir)
    {
        std::string pwd = std::string(::getcwd(nullptr, 256));
        src_dir_ = pwd + std::string(dir);
        HttpContext::srcDir = src_dir_.data();
    }

    void set_thread_num(int thread_num) { tcpserver_->set_thread_num(thread_num); }

private:
    std::unique_ptr<TcpServer> tcpserver_;
    EventLoop *loop_;
    std::string src_dir_;
    WeakConnectionList connection_list_;
};

#endif
