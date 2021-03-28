#include "eventloop.h"
#include "pollbase.h"
#include "channel.h"
#include "epoller.h"

thread_local EventLoop *LoopInThisThread = nullptr;

EventLoop::EventLoop() : looping_(false),
                         quit_(false),
                         pollbase_(new Epoller(this)),
                         thread_id_(std::this_thread::get_id())
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

EventLoop::~EventLoop()
{
    assert(!looping_);
    LoopInThisThread = nullptr;
}

void EventLoop::quit()
{
    quit_ = true;
}

void EventLoop::UpdateChannel(Channel *channel)
{
    // channel中保存的EventLoop是否和channel保存的一样
    assert(channel->ownerLoop() == this);
    // 判断是否当前线程是否与创建时的线程一样
    AssertInLoop();
    pollbase_->UpdateChannel(channel);
}

const EventLoop *EventLoop::getEventOfCurrentThread()
{
    return LoopInThisThread;
}

void EventLoop::loop()
{
    assert(!looping_);
    AssertInLoop();
    looping_ = true;
    quit_ = false;
    while (!quit_)
    {
        active_channels_.clear();
        active_channels_.shrink_to_fit();
        pollbase_->poll(PollTimeMs, &active_channels_);
        for (auto &pItem : active_channels_)
        {
            pItem->HandleEvent();
        }
    }
    sleep(5);
    LOG_INFO("EventLoop %p stop", this);
    looping_ = false;
}

bool EventLoop::IsInLoopThread()
{
    return thread_id_ == std::this_thread::get_id();
}

void EventLoop::AssertInLoop()
{
    if (!IsInLoopThread())
    {
        LOG_ERROR("error");
    }
}