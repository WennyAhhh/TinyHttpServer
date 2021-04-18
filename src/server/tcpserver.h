#pragma once
#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <functional>

class EventLoop;
class Accrptor;
class EventLoopThreadPool;
class TcpConnection;

class TcpServer
{
public:
    typedef std::function<void(EventLoop *)> ThreadInitCallback;
};

#endif