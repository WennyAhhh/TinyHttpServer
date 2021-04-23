#pragma once
#ifndef STAMP_H
#define STAMP_H

// #include "tcpconnection.h"
#include <memory>
#include <functional>

// typedef std::weak_ptr<TcpConnection> WeakConncetionPtr;
typedef std::function<void()> Callback;

template <typename T>
class Stamp
{
public:
    explicit Stamp(std::weak_ptr<T> weak_cb_ptr, const Callback &cb)
        : weak_cb_ptr_(weak_cb_ptr),
          cb_(cb)
    {
    }
    ~Stamp()
    {
        std::shared_ptr<T> ptr = weak_cb_ptr_.lock();
        if (ptr)
        {
            cb_();
        }
    }
private:
    std::weak_ptr<T> weak_cb_ptr_;
    const Callback cb_;
};

#endif
