#pragma once
#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include <vector>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <sys/timerfd.h>
#include "four_heap.h"
#include "base/log.h"
#include "event/channel.h"

class EventLoop;

using std::chrono::high_resolution_clock;

typedef high_resolution_clock::time_point TimerStamp;
class TimerQueue
{
public:
    explicit TimerQueue(EventLoop *loop);
    ~TimerQueue();
    TimerNode add_timer(int node_id, int interval, TimerOutCallBack cb);
    void cancel(int node_id);

private:
    bool insert_();
    std::vector<TimerNode> get_expired_();
    void reset_(int node_id, int timeout);
    void clear_();

    std::unique_ptr<FourHeap> timer_list_;
    const int timerfd_;
    Channel timer_channel_;
    std::atomic<bool> calling_expired_{false};
    EventLoop *loop_;
};

#endif
