#pragma once
#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h>
#include "pollbase.h"
#include "log.h"

class Epoller final : public PollBase
{
public:
    static constexpr int EventListSize = 16;
    Epoller(EventLoop *loop = nullptr);
    ~Epoller() override;

    void poll(int timeoutMs, ChannelList *active_channels) override;

    void update_channel(Channel *channel) override;
    void remove_channel(Channel *channel) override;

protected:
    void fill_active_channels(int num_events, ChannelList *active_channels) const override;
    void update(int epoll_tag, Channel *channel);

private:
    int epollfd_;
    std::vector<epoll_event> events_;
};

#endif