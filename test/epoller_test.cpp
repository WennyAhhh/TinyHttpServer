#include <sys/timerfd.h>
#include <iostream>
#include "event/eventloop.h"
#include "event/channel.h"
#include "event/epoller.h"

using namespace std;

EventLoop *g_loop;

void timeout()
{
    printf("Timer out\n");
    g_loop->quit();
}

int main()
{
    EventLoop loop;
    g_loop = &loop;

    int timefd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(g_loop, timefd);
    channel.set_read_cb(timeout);
    channel.enable_reading();

    itimerspec howlong;
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timefd, 0, &howlong, NULL);
    g_loop->loop();

    return 0;
}