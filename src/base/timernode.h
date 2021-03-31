#pragma once
#ifndef TIMER_NODE_H
#define TIMER_NODE_H

#include <functional>
#include <chrono>

typedef std::function<void()> TimeOutCallBack;
typedef std::chrono::high_resolution_clock Clock;
typedef Clock::time_point TimerStamp;

class TimerNode
{
public:
    int node_id;
    TimerStamp timer;
    TimeOutCallBack cb;
    bool operator==(const TimerNode &rhs)
    {
        // 仅仅判断时间
        return timer == rhs.timer;
    }
    bool operator<(const TimerNode &rhs)
    {
        return timer < rhs.timer;
    }
    bool operator<=(const TimerNode &rhs)
    {
        return timer <= rhs.timer;
    }
    TimerNode &operator=(const TimerNode &rhs)
    {
        node_id = rhs.node_id;
        timer = rhs.timer;
        cb = rhs.cb;
    }
};

#endif
