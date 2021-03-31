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

void EventLoop::update_channel(Channel *channel)
{
    // channel中保存的EventLoop是否和channel保存的一样
    assert(channel->ownerLoop() == this);
    // 判断是否当前线程是否与创建时的线程一样
    assert_in_loop();
    pollbase_->update_channel(channel);
}

const EventLoop *EventLoop::get_curthread()
{
    return LoopInThisThread;
}

void EventLoop::loop()
{
    assert(!looping_);
    assert_in_loop();
    looping_ = true;
    quit_ = false;
    while (!quit_)
    {
        active_channels_.clear();
        active_channels_.shrink_to_fit();
        pollbase_->poll(PollTimeMs, &active_channels_);
        for (auto &pItem : active_channels_)
        {
            pItem->handle_event();
        }
    }
    sleep(5);
    LOG_INFO("EventLoop %p stop", this);
    looping_ = false;
}

bool EventLoop::is_in_loop_thread()
{
    return thread_id_ == std::this_thread::get_id();
}

void EventLoop::assert_in_loop()
{
    if (!is_in_loop_thread())
    {
        LOG_ERROR("error");
    }
}