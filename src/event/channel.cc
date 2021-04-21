#include "channel.h"
#include "eventloop.h"

Channel::Channel(EventLoop *loop, int fd) : loop_(loop),
                                            fd_(fd),
                                            events_(0),
                                            revents_(0),
                                            status_(0)
{
}

Channel::~Channel()
{
    assert(!event_handing_);
    assert(!added_to_loop_);
    // disable_all();
    // remove();
}

void Channel::handle_event()
{
    std::shared_ptr<void> guard;
    if (tied_)
    {
        guard = tie_.lock();
        if (guard)
        {
            handle_event_guard_();
        }
    }
    else
    {
        handle_event_guard_();
    }
}

void Channel::handle_event_guard_()
{
    event_handing_ = true;
    // 分发事件
    // 文件描述符没有打开
    if (revents_ & POLLNVAL)
    {
        LOG_WARN("Channel::handle_event() POLLNVAL");
    }
    if (revents_ & (POLLERR | POLLNVAL))
    {
        if (error_call_back_)
        {
            error_call_back_();
        }
    }
    // 可读事件， 挂起(写端关闭), 带外数据
    if (revents_ & (POLLIN | POLLHUP | POLLPRI))
    {
        if (read_call_back_)
        {
            read_call_back_();
        }
    }
    // 可写事件
    if (revents_ & (POLLOUT | POLLWRBAND))
    {
        if (write_call_back_)
        {
            write_call_back_();
        }
    }

    event_handing_ = false;
}

// update用于更新 PollBase里面的channel， 可以通过channel中的fd进行检索
void Channel::update_()
{
    added_to_loop_ = true;
    loop_->update_channel(this);
}

void Channel::tie(const std::shared_ptr<void> &tie)
{
    //
    tie_ = tie;
    tied_ = true;
}

void Channel::remove()
{
    assert(is_none_event());
    added_to_loop_ = false;
    loop_->remvoe_channel(this);
}