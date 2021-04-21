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
    ~Channel();
    void handle_event();
    void set_read_cb(const EventCallBack &cb) { read_call_back_ = cb; }
    void set_write_cb(const EventCallBack &cb) { write_call_back_ = cb; }
    void set_close_cb(const EventCallBack &cb) { close_call_back_ = cb; }
    void set_error_cb(const EventCallBack &cb) { error_call_back_ = cb; }

    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int _revents) { revents_ = _revents; }
    bool is_none_event() const { return events_ == kNoneEvent; }

    // 设置事件
    void enable_reading()
    {
        events_ |= kReadEvent;
        update_();
    }
    void disable_reading()
    {
        events_ &= ~kReadEvent;
        update_();
    }
    void enable_writing()
    {
        events_ |= kWriteEvent;
        update_();
    }
    void disable_writing()
    {
        events_ &= ~kWriteEvent;
        update_();
    }
    void disable_all()
    {
        events_ = kNoneEvent;
        update_();
    }
    bool is_writing() const { return events_ & kWriteEvent; }
    bool is_reading() const { return events_ & kReadEvent; }

    // channel在epoller的状态
    int status() { return status_; }
    void set_status(int status) { status_ = status; }

    EventLoop *ownerLoop() { return loop_; }

    void tie(const std::shared_ptr<void> &tie);
    void remove();

private:
    void update_();
    void handle_event_guard_();

    EventLoop *loop_;
    const int fd_;
    int events_;
    int revents_;
    int status_;
    bool tied_;
    std::weak_ptr<void> tie_;

    bool event_handing_{false};
    bool added_to_loop_{false};
    EventCallBack read_call_back_;
    EventCallBack write_call_back_;
    EventCallBack close_call_back_;
    EventCallBack error_call_back_;
};
#endif