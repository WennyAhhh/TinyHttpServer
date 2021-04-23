#pragma once
#ifndef HTTPSERVER_ENTRY_H
#define HTTPSERVER_ENTRY_H

// #include "tcpconnection.h"
#include <memory>
#include <functional>
#include "server/tcpconnection.h"

typedef std::weak_ptr<TcpConnection> WeakTcpConnectionPtr;

class Entry
{
public:
    explicit Entry(const WeakTcpConnectionPtr &weakConn)
        : weakConn_(weakConn)
    {
    }

    ~Entry()
    {
        TcpConnectionPtr conn = weakConn_.lock();
        if (conn)
        {
            conn->shutdown();
        }
    }

private:
    WeakTcpConnectionPtr weakConn_;
};

#endif
