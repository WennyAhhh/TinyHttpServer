#include "eventloop.h"

thread_local EventLoop *LoopInThisThread = nullptr;

EventLoop::EventLoop() : looping_(false),
                         thread_id(std::this_thread::get_id())
{
    LOG_INFO("EventLoop created %p", this);
    if (LoopInThisThread != nullptr)
    {
        printf("is not nullptr\n");
        LOG_ERROR("Another EventLoop %p exits in this thread ", LoopInThisThread);
    }
    else
    {
        LoopInThisThread = this;
    }
}

const EventLoop *EventLoop::getEventOfCurrentThread()
{
    return LoopInThisThread;
}

EventLoop::~EventLoop()
{
    assert(!looping_);
    LoopInThisThread = nullptr;
}

void EventLoop::loop()
{
    assert(!looping_);
    AssertInLoop();
    looping_ = true;
    //::poll(NULL, 0, 2);
    sleep(5);
    Log::Instance()->stop();
    LOG_INFO("EventLoop %p stop", this);
    looping_ = false;
}

bool EventLoop::IsInLoopThread()
{
    return thread_id == std::this_thread::get_id();
}

void EventLoop::AssertInLoop()
{
    if (!IsInLoopThread())
    {
    }
}