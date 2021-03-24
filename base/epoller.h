#pragma once
#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h>
#include "pollbase.h"
#include "../src/log.h"

class Epoller final : public PollBase
{
public:
    static constexpr int EventListSize = 16;
    Epoller(EventLoop *loop = nullptr);
    ~Epoller() override;

    void poll(int timeoutMs, ChannelList *active_channels) override;

    void UpdateChannel(Channel *channel) override;

    void RemoveChannel(Channel *channel) override;

protected:
    void FillActiveChannels(int num_events, ChannelList *active_channels) const override;
    void update(int epoll_tag, Channel *channel);

private:
    int epollfd_;
    std::vector<epoll_event> events_;
};

#endif