#pragma once
#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include <thread>
#include <memory>
#include <unistd.h>
#include <cassert>
#include <sys/poll.h>
#include "../src/log.h"

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void AssertInLoop();
    bool IsInLoopThread();
    const EventLoop *EventLoop::getEventOfCurrentThread();

private:
    std::thread::id thread_id;
    bool looping_;
};
#endif