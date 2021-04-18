#pragma once
#ifndef EVENT_LOOP_THREAD_H
#define EVENT_LOOP_THREAD_H

#include <condition_variable>
#include <thread>
#include <mutex>
#include <functional>

class EventLoop;

class EventLoopThread
{
public:
    typedef std::function<void(EventLoop *)> ThreadInitCallback;
    EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(), const std::string &name = nullptr);
    ~EventLoopThread();
    EventLoop *start_loop();

private:
    void thread_func_();
    std::atomic<EventLoop *> loop_;
    bool exiting_{false};
    std::string name_;

    std::thread thread_;
    // 用自旋锁是否会更好？
    // std::mutex mtx_;
    // std::condition_variable cond_;
    ThreadInitCallback thread_cb_;
};

#endif