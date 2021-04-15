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
    int64_t micro = static_cast<int64_t>(std::chrono::duration<double, std::micro>(timer.get_timer() - TimerNode::now()).count());
    if (micro < 100)
    {
        micro = 100;
    }
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(micro / TimerNode::kmicrosecond);
    ts.tv_nsec = static_cast<long>((micro % TimerNode::kmicrosecond) * 1000);
    return ts;
}

void read_timer_fd(int timerfd, TimerNode now)
{
    uint64_t t;
    ssize_t n = ::read(timerfd, &t, sizeof t);
    LOG_DEBUG("TImerQueue::handle_read() %llu ", t);
    if (n != sizeof t)
    {
        LOG_ERROR("TImerQueue::handle_read() reads %u", n);
    }
}

void reset_timerfd(int timerfd, TimerNode expiration)
{
    itimerspec new_val;
    itimerspec old_val;
    memset(&new_val, 0, sizeof new_val);
    memset(&old_val, 0, sizeof old_val);
    new_val.it_value = time_from_now(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &new_val, &old_val);
    if (ret != 0)
    {
        LOG_DEBUG("timerfd_settime()");
    }
}

TimerQueue::TimerQueue(EventLoop *loop) : loop_(loop),
                                          timerfd_(create_timerfd()),
                                          timer_channel_(loop, timerfd_),
                                          timer_list_(std::make_unique<FourHeap>())
{
    timer_channel_.set_read_cb(std::bind(&TimerQueue::handle_read_, this));
    timer_channel_.enable_reading();
}

TimerQueue::~TimerQueue()
{
    clear_();
    timer_channel_.disable_all();
    timer_channel_.remove();
    ::close(timerfd_);
}

TimerNode TimerQueue::add_timer(float interval, TimerOutCallBack cb, bool repeat)
{
    int node_seq = get_();
    assert(node_seq >= 0);
    // 转为微秒级
    TimerStamp timer = TimerNode::now() + TimerNode::tarns_mirco(interval);
    TimerNode node(node_seq, interval, timer, cb, repeat);
    loop_->run_in_loop(std::bind(&TimerQueue::add_timer_in_loop_, this, node));
    return node;
}

void TimerQueue::clear_()
{
    timer_list_->clear();
    cancel_set_.clear();
}

void TimerQueue::cancel(TimerNode node)
{
    loop_->run_in_loop(std::bind(&TimerQueue::cancel_timer_in_loop_, this, node));
}

void TimerQueue::add_timer_in_loop_(TimerNode node)
{
    loop_->assert_in_loop();
    timer_list_->push(node);
    if (node <= timer_list_->front())
    {
        reset_timerfd(timerfd_, node);
    }
}

void TimerQueue::cancel_timer_in_loop_(TimerNode node)
{
    loop_->assert_in_loop();
    if (timer_list_->find(node))
    {
        timer_list_->remove(node);
    }
    else if (calling_expired_)
    {
        // 存在嵌套
        cancel_set_.insert(node);
    }
}

void TimerQueue::handle_read_()
{
    loop_->assert_in_loop();
    std::vector<TimerNode> expired = get_expired_();
    is_cancel_ = true;
    cancel_set_.clear();
    for (const auto &e : expired)
    {
        e.run();
    }
    is_cancel_ = false;

    reset_(expired);
}

void TimerQueue::reset_(std::vector<TimerNode> &expired)
{
    // 暂时只支持延长
    for (auto &e : expired)
    {
        if (e.get_repeat_() && cancel_set_.find(e) == cancel_set_.end())
        {
            TimerStamp timer = TimerNode::now() + TimerNode::tarns_mirco(e.get_interval());
            e.set_timer_stamp(timer);
            // e.set_timer_stamp(Clock::now() + e.get_interval());
            timer_list_->push(e);
        }
    }
    if (!timer_list_->empty())
    {
        TimerNode new_expire = timer_list_->front();
        reset_timerfd(timerfd_, new_expire);
    }
}

std::vector<TimerNode> TimerQueue::get_expired_()
{
    TimerStamp ts = TimerNode::now();
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