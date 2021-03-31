#include "pollbase.h"
#include "channel.h"
#include "eventloop.h"

void PollBase::assert_in_loop_thread() const
{
    ownerLoop_->is_in_loop_thread();
}

bool PollBase::has_channel(Channel *channel) const
{
    assert_in_loop_thread();
    auto pItem = channels_.find(channel->fd());
    return pItem != channels_.end() && pItem->second == channel;
}