
#include "eventloopthread.h"
#include "event/eventloop.h"

EventLoopThread::EventLoopThread(const ThreadInitCallback &cb, const std::string &name)
    : thread_cb_(cb),
      name_(name),
      loop_(nullptr)
{
}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    // muduo中thread_.join()与loop_->quit()放在一起
    // thread_func中的call_back使调用quit使loop退出， 此时loop_ = nullptr
    // 如果之后不再进行线程的分配， eventloop_不进行赋值， 那么线程将无法调用join()
    if (loop_.load() != nullptr)
    {
        EventLoop *loop = loop_.load();
        loop->quit();
    }
    if (thread_.joinable())
    {
        thread_.join();
    }
}

EventLoop *EventLoopThread::start_loop()
{
    thread_ = std::thread(std::bind(&EventLoopThread::thread_func_, this), name_);

    EventLoop *loop = nullptr;
    while (loop_.load() == nullptr)
        ;
    loop = loop_.load();

    return loop;
}

void EventLoopThread::thread_func_()
{
    EventLoop loop;
    if (thread_cb_)
    {
        thread_cb_(&loop);
    }
    loop_.store(&loop);
    loop.loop();
    loop_.store(nullptr);
}