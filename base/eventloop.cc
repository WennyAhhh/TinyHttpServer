#include "eventloop.h"

thread_local std::shared_ptr<EventLoop> LoopInThisThread = nullptr;

EventLoop::EventLoop() : looping_(false),
                         thread_id(std::this_thread::get_id())
{
    LOG_INFO("EventLoop created %p %lld", this, thread_id);
    if (LoopInThisThread != nullptr)
    {
        LOG_ERROR("Another EventLoop %s exits in this thread %lld ", LoopInThisThread, thread_id);
    }
    else
    {
        LoopInThisThread = std::unique_ptr<EventLoop>(this);
    }
}

EventLoop::~EventLoop()
{
    assert(!looping_);
    // 对月thread_local， 如果线程结束了，那么自然也会因为智能指针的原因释放
}

std::shared_ptr<EventLoop> EventLoop::GetEventLoopCurrentThread()
{
    return LoopInThisThread;
}

bool EventLoop::IsInLoopThread()
{
    return thread_id == std::this_thread::get_id();
}

void EventLoop::AssertInLoop()
{
    if (!IsInLoopThread())
    {
        AbortNotLoopThread();
    }
}