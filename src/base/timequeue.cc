#include "timequeue.h"

TimerQueue::TimerQueue(EventLoop *loop) : loop_(loop)
{
}

TimerQueue::~TimerQueue()
{
    clear();
}

TimerNode TimerQueue::add_timer(int node_id, int interval, TimerOutCallBack cb)
{
    assert(node_id >= 0);
    TimerNode node({node_id, Clock::now() + std::chrono::milliseconds(interval), cb});
    timer_list.push(node_id, node);
    return node;
}

void TimerQueue::clear()
{
    timer_list.clear();
}

void TimerQueue::del(int node_id)
{
    timer_list.remove(node_id);
}

void TimerQueue::reset(int node_id, int timeout)
{
    // 暂时只支持延长
    timer_list.reset(node_id, Clock::now() + std::chrono::milliseconds(timeout));
}

std::vector<TimerNode> TimerQueue::get_expired()
{
    auto ts = Clock::now();
    std::vector<TimerNode> res;
    while (!timer_list.empty())
    {
        TimerNode tar = timer_list.front();
        if (tar.timer > ts)
            break;
        res.emplace_back(tar);
        timer_list.pop();
    }
    return res;
}