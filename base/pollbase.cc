#include "pollbase.h"
#include "channel.h"
#include "eventloop.h"

void PollBase::AssertInLoopThread() const
{
    ownerLoop_->IsInLoopThread();
}

bool PollBase::HasChannel(Channel *channel) const
{
    AssertInLoopThread();
    auto pItem = channels_.find(channel->fd());
    return pItem != channels_.end() && pItem->second == channel;
}