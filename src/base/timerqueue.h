#pragma once
#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include <queue>
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
    static const int LIMIT = 1024;
    explicit TimerQueue(EventLoop *loop);
    ~TimerQueue();
    TimerNode add_timer(float interval, TimerOutCallBack cb);
    void cancel(TimerNode node);

private:
    bool insert_();
    std::vector<TimerNode> get_expired_();
    void reset_(int node_id, int timeout);
    void add_timer_in_loop(TimerNode);
    void cancel_timer_in_loop(TimerNode);
    int get_();
    void del_(TimerNode &);
    void clear_();

    std::unique_ptr<FourHeap> timer_list_;
    std::queue<int> seq_;
    size_t seq_limit_;
    size_t index_;
    const int timerfd_;
    Channel timer_channel_;
    std::atomic<bool> calling_expired_{false};
    EventLoop *loop_;
};

#endif
