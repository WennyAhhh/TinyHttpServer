#pragma once
#ifndef CHANNEL_H
#define CHANNEL_H
#include <functional>
#include <sys/poll.h>
#include "base/log.h"
class EventLoop;

// 事件分发
class Channel
{
public:
    static const int kNoneEvent = 0;
    static const int kReadEvent = POLLIN | POLLPRI;
    static const int kWriteEvent = POLLOUT;
    typedef std::function<void()> EventCallBack;
    Channel(EventLoop *loop, int fd);

    void handle_event();
    void set_read_cb(const EventCallBack &cb)
    {
        read_call_back_ = cb;
    }
    void set_write_cb(const EventCallBack &cb)
    {
        write_call_back_ = cb;
    }
    void set_error_cb(const EventCallBack &cb)
    {
        error_call_back_ = cb;
    }
    int fd() const
    {
        return fd_;
    }
    int events() const
    {
        return events_;
    }

    void set_revents(int _revents)
    {
        revents_ = _revents;
    }
    bool is_none_event() const
    {
        return events_ == kNoneEvent;
    }
    void enable_reading()
    {
        events_ |= kReadEvent;
        update();
    }

    int index()
    {
        return index_;
    }
    void set_index(int idx)
    {
        index_ = idx;
    }
    EventLoop *ownerLoop()
    {
        return loop_;
    }

private:
    void update();

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_;
    // 记录的fd
    const int fd_;
    // 事件的类型
    int events_;
    //
    int revents_;
    // channel在PollBase中拥有的下标
    int index_;

    EventCallBack read_call_back_;
    EventCallBack write_call_back_;
    EventCallBack error_call_back_;
};
#endif