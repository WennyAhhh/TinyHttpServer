#include <iostream>
#include "event/eventloop.h"

using namespace std;

EventLoop *g_loop;

void print()
{
    cout << "xiao mi print" << endl;
}

void cancel(TimerNode timer)
{
    g_loop->cancel(timer);
    cout << "cancel" << endl;
}

int main()
{
    std::cout << "xx" << std::endl;
    Log::instance()->init("./.log", true, true);
    EventLoop loop;
    g_loop = &loop;
    TimerNode id = loop.run_after(2, std::bind(print));
    // loop.run_after(2, std::bind(print));
    TimerNode c = loop.run_after(3, std::bind(cancel, id), false);
    loop.loop();

    return 0;
}