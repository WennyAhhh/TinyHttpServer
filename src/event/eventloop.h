#pragma once
#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include <thread>
#include <memory>
#include <atomic>
#include <unistd.h>
#include <cassert>
#include <sys/poll.h>
#include "../base/log.h"

class Channel;
class PollBase;

class EventLoop
{
public:
    static constexpr int PollTimeMs = 10000;
    EventLoop();
    ~EventLoop();

    void loop();
    void assert_in_loop();
    bool is_in_loop_thread();
    void quit();

    void update_channel(Channel *channel);
    const EventLoop *EventLoop::get_curthread();

private:
    typedef std::vector<Channel *> ChannelList;
    // 如果暴露给其他线程就需要是原子的
    std::atomic<bool> quit_;
    std::thread::id thread_id_;
    std::unique_ptr<PollBase> pollbase_;
    ChannelList active_channels_;
    bool looping_;
};
#endif