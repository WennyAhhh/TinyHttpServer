#include "epoller.h"
#include "channel.h"

Epoller::Epoller(EventLoop *loop) : PollBase(loop),
                                    epollfd_(epoll_create1(EPOLL_CLOEXEC)),
                                    events_(EventListSize)
{
    if (epollfd_ < 0)
    {
        LOG_ERROR("EPoller::Epoller");
    }
}

Epoller::~Epoller()
{
    ::close(epollfd_);
}

void Epoller::poll(int timeoutMs, ChannelList *active_channels)
{
    int cnt = epoll_wait(epollfd_, &*events_.data(), static_cast<int>(events_.size()), timeoutMs);
    int saved_errno = errno;
    if (cnt > 0)
    {
        if (cnt >= 5)
        {
            LOG_INFO("%d events happend", cnt);
        }
        fill_active_channels(cnt, active_channels);
        if (cnt == events_.size())
        {
            events_.resize(cnt * 2);
        }
    }
    else if (cnt == 0)
    {
        LOG_INFO("nothing happended");
    }
    else
    {
        if (saved_errno != EINTR)
        {
            errno = saved_errno;
            LOG_ERROR("EPollPoller::poll()");
        }
    }
}

void Epoller::fill_active_channels(int num_events, ChannelList *active_channels) const
{
    for (int i = 0; i < num_events; i++)
    {
        // 联合体， 其中ptr就是channel， 其中包含了fd
        Channel *channel = static_cast<Channel *>(events_[i].data.ptr);
        // int fd = channel->fd();
        // 保存上一次调用的事件
        channel->set_revents(events_[i].events);
        active_channels->push_back(channel);
    }
}

void Epoller::update_channel(Channel *channel)
{
    // 更新
    PollBase::assert_in_loop_thread();
    const int status = channel->status();
    int fd = channel->fd();
    LOG_INFO("fd = %d events = %d index = %d", channel->fd(), channel->events(), channel->status());
    if (status == static_cast<int>(POLL_TAG::NEW) || status == static_cast<int>(POLL_TAG::DEL))
    {
        // 如果channel没有加入到map中, 需要加入
        channel->set_status(static_cast<int>(POLL_TAG::ADD));
        int fd = channel->fd();
        if (status == static_cast<int>(POLL_TAG::NEW))
        {
            auto pItem = channels_.find(fd);
            assert(pItem == channels_.end());
            // channel[fd] =  channel;
            channels_.insert({fd, channel});
        }
        else
        {
            // 只是标记事件在epoll内核中被删除，但map没有删除
            auto pItem = channels_.find(fd);
            assert(pItem == channels_.end());
            assert(pItem->second == channel);
        }
        channel->set_status(static_cast<int>(POLL_TAG::ADD));
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        if (channel->is_none_event())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_status(static_cast<int>(POLL_TAG::DEL));
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void Epoller::update(int epoll_tag, Channel *channel)
{
    epoll_event event;
    memset(&event, 0, sizeof event);
    event.data.ptr = channel;
    event.events = channel->events();
    int fd = channel->fd();
    if ((epoll_ctl(epollfd_, epoll_tag, fd, &event)) < 0)
    {
        if (epoll_tag == EPOLL_CTL_DEL)
        {
            LOG_INFO("EPOLL_CTL_DEL");
        }
        else if (epoll_tag == EPOLL_CTL_MOD)
        {
            LOG_INFO("EPOLL_CTL_MOD");
        }
    }
}

void Epoller::remove_channel(Channel *channel)
{
    PollBase::assert_in_loop_thread();
    int fd = channel->fd();
    LOG_INFO(" fd = %d", fd);
    int index = channel->status();
    assert(index == static_cast<int>(POLL_TAG::ADD) || index == static_cast<int>(POLL_TAG::DEL));
    channels_.erase(fd);
    if (index == static_cast<int>(POLL_TAG::ADD))
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_status(static_cast<int>(POLL_TAG::NEW));
}