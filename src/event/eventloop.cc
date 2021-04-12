#include "eventloop.h"
#include "pollbase.h"
#include "channel.h"
#include "epoller.h"

thread_local EventLoop *LoopInThisThread = nullptr;

int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG_ERROR("Failed in eventfd");
        abort();
    }
    return evtfd;
}

EventLoop::EventLoop() : pollbase_(new Epoller(this)),
                         thread_id_(std::this_thread::get_id()),
                         wakefd_(createEventfd()),
                         wake_channel(std::make_unique<Channel>(this, wakefd_)),
                         timer_queue_(std::make_unique<TimerQueue>(this))
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
    wake_channel->set_read_cb(std::bind(&EventLoop::handle_read_, this));
    wake_channel->enable_reading();
}

EventLoop::~EventLoop()
{
    assert(!looping_);
    ::close(wakefd_);
    LoopInThisThread = nullptr;
}

void EventLoop::quit()
{
    quit_ = true;
    if (!is_in_loop_thread())
    {
        wakeup_();
    }
}

void EventLoop::update_channel(Channel *channel)
{
    // channel中保存的EventLoop是否和channel保存的一样
    assert(channel->ownerLoop() == this);
    // 判断是否当前线程是否与创建时的线程一样
    assert_in_loop();
    pollbase_->update_channel(channel);
}

void EventLoop::remvoe_channel(Channel *channel)
{
    assert(channel->ownerLoop() == this);
    assert_in_loop();
    if (event_handling)
    {
        // 如果此时事件正在处理， 需要判断移除的是否的fd是否还在处理
        assert()
    }
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
        // 因为返回的是指针， 用引用好像有些不合适
        for (Channel *channel : active_channels_)
        {
            cur_channel = channel;
            channel->handle_event();
        }
        cur_channel = nullptr;
        dopending_func();
    }
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

TimerNode EventLoop::run_after(int node_id, int delay, TimerOutCallBack cb)
{
    return timer_queue_->add_timer(node_id, delay, cb);
}

void EventLoop::cancel(int node_id)
{
    timer_queue_->cancel(node_id);
}

void EventLoop::run_in_loop(Functor cb)
{
    if (is_in_loop_thread())
    {
        cb();
    }
    else
    {
        queue_in_loop_(std::move(cb));
    }
}

void EventLoop::queue_in_loop_(Functor cb)
{
    {
        std::lock_guard<std::mutex> lk(mtx_);
        pending_func_.push_back(cb);
    }
    if (!is_in_loop_thread() || calling_)
    {
        wakeup_();
    }
}

void EventLoop::wakeup_()
{
    ssize_t wake_data = 1;
    if (::write(wakefd_, &wake_data, sizeof wake_data) != 1)
    {
        LOG_ERROR("EventLoop::wakeup() ");
    }
}

void EventLoop::handle_read_()
{
    ssize_t wake_data = 1;
    if (::read(wakefd_, &wake_data, sizeof wake_data) != 1)
    {
        LOG_ERROR("EventLoop::read()");
    }
}

void EventLoop::dopending_func()
{
    std::vector<Functor> functors;
    calling_ = true;
    {
        std::lock_guard<std::mutex> lk(mtx_);
        functors.swap(pending_func_);
    }

    for (const Functor &functor : functors)
    {
        functor();
    }
    calling_ = false;
}
