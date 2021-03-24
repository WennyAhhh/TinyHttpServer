#include "channel.h"
#include "eventloop.h"

Channel::Channel(EventLoop *loop, int fd) : loop_(loop),
                                            fd_(fd),
                                            events_(0),
                                            revents_(0),
                                            index_(-1)
{
}

void Channel::HandleEvent()
{
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
}

// update用于更新 poller里面的channel， 可以通过channel中的fd进行检索
void Channel::update()
{
    //loop_->updateChannel(this);
}