#include "eventloopthread.h"
#include "event/eventloop.h"
#include "eventloopthreadpool.h"
EventLoopThreadPool::EventLoopThreadPool(EventLoop *base_loop, const std::string &nameArg)
    : base_loop_(base_loop),
      name_(nameArg)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
    // 主线程的loop， 不需要释放
}

void EventLoopThreadPool::start(const ThreadInitCallback &cb)
{
    for (int i = 0; i < num_threads_; i++)
    {
        std::string thread_name = name_ + "_" + std::to_string(i);
        auto thread_event_loop = std::make_unique<EventLoopThread>(cb, thread_name);
        loops_.push_back(thread_event_loop->start_loop());
        threads_.emplace_back(thread_event_loop);
    }
    if (num_threads_ == 0)
    {
        cb(base_loop_);
    }
}

EventLoop *EventLoopThreadPool::get_loop()
{
    base_loop_->assert_in_loop();
    assert(started_);
    auto loop = base_loop_;

    if (loops_.empty())
    {
        loop = loops_[next_++];
        if (loops_.size() <= next_)
        {
            next_ = 0;
        }
    }
    return loop;
}