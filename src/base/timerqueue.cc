#include "timerqueue.h"
#include "event/eventloop.h"

int create_timerfd()
{
    // 不收系统时间的更改而更改， 非阻塞， 在子进程执行exec之后关闭。
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0)
    {
        LOG_WARN("Failed in timerfd create");
    }
    return timerfd;
}

timespec time_from_now(TimerNode timer)
{
}

TimerQueue::TimerQueue(EventLoop *loop) : loop_(loop),
                                          timerfd_(create_timerfd()),
                                          timer_channel_(loop_, timerfd_),
                                          timer_list_(std::make_unique<FourHeap>())
{
    timer_channel_;
}

TimerQueue::~TimerQueue()
{
    clear_();
}

TimerNode TimerQueue::add_timer(float interval, TimerOutCallBack cb)
{
    int node_seq = get_();
    assert(node_seq >= 0);
    TimerStamp timer = TimerNode::now() + TimerNode::milliseconds(interval);
    TimerNode node(node_seq, timer, cb);
    loop_->run_in_loop(std::bind(&TimerQueue::add_timer_in_loop, this, node));
    return node;
}

void TimerQueue::clear_()
{
    timer_list_->clear();
}

void TimerQueue::cancel(TimerNode node)
{
    loop_->run_in_loop(std::bind(&TimerQueue::cancel_timer_in_loop, this, node));
}

void TimerQueue::add_timer_in_loop(TimerNode node)
{
    loop_->assert_in_loop();
    timer_list_->push(node);
}

void TimerQueue::cancel_timer_in_loop(TimerNode node)
{
}

void TimerQueue::reset_(int node_id, int timeout)
{
    // 暂时只支持延长
}

std::vector<TimerNode> TimerQueue::get_expired_()
{
    auto ts = Clock::now();
    std::vector<TimerNode> res;
    while (!timer_list_->empty())
    {
        TimerNode tar = timer_list_->front();
        if (tar.get_timer() > ts)
            break;
        res.emplace_back(tar);
        timer_list_->pop();
    }
    return res;
}

int TimerQueue::get_()
{
    int seq = -1;
    if (seq_.empty())
    {
        for (int i = 0; i < LIMIT; i++)
        {
            ++seq_limit_;
            ++index_;
            seq_.push(seq_limit_);
        }
    }
    seq = seq_.front();
    seq_.pop();
    return seq;
}

void TimerQueue::del_(TimerNode &node)
{
    int seq = node.get_node_seq();
    seq_.push(seq);
    if (index_ == seq_limit_)
    {
        seq_limit_ = 0;
        index_ = 0;
        std::queue<int> tar;
        swap(seq_, tar);
    }
    else
    {
        index_--;
    }
}