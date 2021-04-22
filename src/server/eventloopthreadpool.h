#pragma once
#ifndef EVENT_LOOP_THREAD_POOL_H
#define EVENT_LOOP_THREAD_POOL_H

#include <functional>
#include <memory>
#include <vector>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
public:
    typedef std::function<void(EventLoop *)> ThreadInitCallback;

    EventLoopThreadPool(EventLoop *base_loop, const std::string &nameArg);
    ~EventLoopThreadPool();

    void set_thread_num(int num_threads) { num_threads_ = num_threads; }
    bool get_started() const { return started_; }
    const std::string &get_name() const { return name_; }

    void start(const ThreadInitCallback &cb = ThreadInitCallback());
    EventLoop *get_loop();

private:
    int num_threads_{0};
    std::string name_;
    bool started_{false};
    int next_{0};

    EventLoop *base_loop_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop *> loops_;
};

#endif