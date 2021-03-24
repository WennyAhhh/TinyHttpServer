#pragma once
#ifndef POLLBASE_H
#define POLLBASE_H

#include <vector>
#include <map>
#include <unistd.h>

class Channel;
class EventLoop;

class PollBase
{
public:
    enum class POLL_TAG
    {
        NEW = 0,
        ADD,
        DEL
    };
    typedef std::vector<Channel *> ChannelList;
    typedef std::map<int, Channel *> ChannelMap;

    PollBase(EventLoop *loop) : ownerLoop_(loop) {}
    virtual ~PollBase() = default;

    virtual void poll(int timeoutMs, ChannelList *ActiveChannels) = 0;

    virtual void UpdateChannel(Channel *channel) = 0;

    virtual void RemoveChannel(Channel *channel) = 0;

    virtual bool HasChannel(Channel *channel) const;

    void AssertInLoopThread() const;

protected:
    virtual void FillActiveChannels(int num_events, ChannelList *active_channels) const = 0;
    ChannelMap channels_;

private:
    EventLoop *ownerLoop_;
};

#endif