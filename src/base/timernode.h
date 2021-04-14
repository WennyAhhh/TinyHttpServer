#pragma once
#ifndef TIMER_NODE_H
#define TIMER_NODE_H

#include <functional>
#include <chrono>

typedef std::function<void()> TimerOutCallBack;
typedef std::chrono::steady_clock Clock;
typedef std::chrono::steady_clock::time_point TimerStamp;

// todo 模板， 移动构造
class TimerNode
{
public:
    static constexpr int kmillisecond = 1000;
    // 转化成为毫秒
    static std::chrono::microseconds tarns_mirco(float interval)
    {
        return std::chrono::microseconds(static_cast<int64_t>(interval * kmillisecond));
    }
    TimerNode(int node_seq, TimerStamp &timer, TimerOutCallBack &cb) : node_seq_(node_seq),
                                                                       timer_(std::move(timer)),
                                                                       cb_(std::move(cb))
    {
    }

    static TimerStamp now()
    {
        return Clock::now();
    }

    void run()
    {
        cb_();
    }

    const int get_node_seq()
    {
        return node_seq_;
    }

    TimerStamp get_timer()
    {
        return timer_;
    }

    void set_call_back(TimerOutCallBack &cb)
    {
        cb_ = std::move(cb);
    }

    void set_timer_stamp(TimerStamp &timer)
    {
        timer_ = std::move(timer);
    }

    bool operator==(const TimerNode &rhs)
    {
        // 仅仅判断时间
        return timer_ == rhs.timer_;
    }
    bool operator<(const TimerNode &rhs)
    {
        return timer_ < rhs.timer_;
    }
    bool operator<=(const TimerNode &rhs)
    {
        return timer_ <= rhs.timer_;
    }
    TimerNode &operator=(const TimerNode &rhs)
    {
        node_seq_ = rhs.node_seq_;
        timer_ = rhs.timer_;
        cb_ = rhs.cb_;
    }

private:
    int node_seq_;
    TimerStamp timer_;
    TimerOutCallBack cb_;
};

#endif
