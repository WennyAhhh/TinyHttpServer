#include <unistd.h>
#include <iostream>

#include "server/eventloopthreadpool.h"
#include "event/eventloop.h"
#include "base/log.h"

using namespace std;

EventLoop *g_loop;

void print(EventLoop *baseloop)
{
    cout << "xiao mi" << endl;
}

void test(int maxSize)
{
    LOG_DEBUG("Test ThreadPool with max queue size = %d", maxSize);
    EventLoopThreadPool pool(g_loop, "MainTreadPool");
    pool.set_thread_num(maxSize);
    LOG_WARN("Adding");
    pool.start(print);
    LOG_WARN("Done");
}

int main()
{
    std::cout << "xx" << std::endl;
    Log::instance()->init("./.log", true, true);
    EventLoop loop;
    g_loop = &loop;
    test(5);
    loop.loop();
}