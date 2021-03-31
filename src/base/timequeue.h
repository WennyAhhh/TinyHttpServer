#pragma once
#ifndef TIMEQUEUE_H
#define TIMEQUEUE_H

#include <set>
#include <vector>
#include <chrono>
#include <functional>
#include <unordered_map>

#include "../event/eventloop.h"
#include "four_heap.h"

using std::chrono::high_resolution_clock;

typedef high_resolution_clock::time_point TimerStamp;
typedef std::function<void()> TimeroutCallBack;
class TimerQueue
{
public:
    explicit TimerQueue(EventLoop *loop);
    ~TimerQueue();
    void add_timer(int node_id, int interval, TimeroutCallBack cb);
    void del(int node_id);
    std::vector<TimerNode> get_expired();
    void reset(int node_id, int timeout);
    void clear();

private:
    FourHeap timer_list;
    EventLoop *loop_;
};

#endif
