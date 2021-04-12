#include "timerqueue.h"

int create_timerfd()
{
    // 不收系统时间的更改而更改， 非阻塞， 在子进程执行exec之后关闭。
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0)
    {
        LOG_WARN("Failed in timerfd create");
    }
    return timerfd;
}

timespec time_from_now(TimerNode timer)
{
}

TimerQueue::TimerQueue(EventLoop *loop) : loop_(loop),
                                          timerfd_(create_timerfd()),
                                          timer_channel_(loop_, timerfd_),
                                          timer_list_(std::make_unique<FourHeap>())
{
    timer_channel_;
}

TimerQueue::~TimerQueue()
{
    clear_();
}

TimerNode TimerQueue::add_timer(int node_id, int interval, TimerOutCallBack cb)
{
    assert(node_id >= 0);
    TimerNode node({node_id, Clock::now() + std::chrono::milliseconds(interval), cb});
    timer_list_->push(node_id, node);
    return node;
}

void TimerQueue::clear_()
{
    timer_list_->clear();
}

void TimerQueue::cancel(int node_id)
{
    timer_list_->remove(node_id);
}

void TimerQueue::reset_(int node_id, int timeout)
{
    // 暂时只支持延长
    timer_list_->reset(node_id, Clock::now() + std::chrono::milliseconds(timeout));
}

std::vector<TimerNode> TimerQueue::get_expired_()
{
    auto ts = Clock::now();
    std::vector<TimerNode> res;
    while (!timer_list_->empty())
    {
        TimerNode tar = timer_list_->front();
        if (tar.timer > ts)
            break;
        res.emplace_back(tar);
        timer_list_->pop();
    }
    return res;
}