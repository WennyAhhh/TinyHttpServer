#pragma once
#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include <thread>
#include <memory>
#include <atomic>
#include <unistd.h>
#include <cassert>
#include <sys/poll.h>
#include <sys/eventfd.h>
#include "base/log.h"
#include "base/timerqueue.h"

class Channel;
class PollBase;

typedef std::function<void()> Functor;

class EventLoop
{
public:
    static constexpr int PollTimeMs = 10000;
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();
    void assert_in_loop();
    bool is_in_loop_thread();
    //
    void run_in_loop(Functor cb);
    void queue_in_loop(Functor cb);

    void update_channel(Channel *channel);
    void remvoe_channel(Channel *channel);
    const EventLoop *get_curthread();
    TimerId run_after(float delay, TimerOutCallBack cb, bool repeat = true);
    TimerId extend_timer(float delay, TimerId id, bool repeat = true);
    void cancel(TimerId);

private:
    typedef std::vector<Channel *> ChannelList;
    void wakeup_();
    void handle_read_();
    void dopending_func();
    // 如果暴露给其他线程就需要是原子的
    std::atomic<bool> quit_{false};
    std::atomic<bool> calling_{false};
    std::atomic<bool> event_handling{false};
    std::mutex mtx_;
    std::thread::id thread_id_;
    std::unique_ptr<PollBase> poller_;
    std::unique_ptr<TimerQueue> timer_queue_;
    std::unique_ptr<Channel> wake_channel_;
    std::vector<Functor> pending_func_;

    Channel *cur_channel_;
    ChannelList active_channels_;
    bool looping_{false};
    int wakefd_;
};
#endif