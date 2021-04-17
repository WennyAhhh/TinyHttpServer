#pragma once
#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include <queue>
#include <vector>
#include <chrono>
#include <set>
#include <functional>
#include <unordered_map>
#include <sys/timerfd.h>
#include "four_heap.h"
#include "base/log.h"
#include "event/channel.h"

class EventLoop;

class TimerQueue
{
public:
    static const int LIMIT = 1024;
    explicit TimerQueue(EventLoop *loop);
    ~TimerQueue();
    TimerId add_timer(float interval, TimerOutCallBack cb, bool repeat);
    TimerId extend_timer(float interval, TimerId id, bool repeat);
    void cancel(TimerId id);

private:
    std::vector<TimerNode> get_expired_();
    void reset_(std::vector<TimerNode> &);
    void add_timer_in_loop_(TimerNode);
    void cancel_timer_in_loop_(TimerId);
    void extend_timer_in_loop_(float, TimerId, bool repeat);
    void handle_read_();
    int get_();
    void del_(TimerNode &);
    void clear_();

    std::unique_ptr<FourHeap> timer_list_;
    std::set<TimerId> cancel_set_;
    std::queue<int> seq_;
    size_t seq_limit_;
    size_t index_;
    const int timerfd_;
    Channel timer_channel_;
    std::atomic<bool> calling_expired_{false};
    EventLoop *loop_;
};

#endif
