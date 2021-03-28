#include <sys/timerfd.h>
#include <iostream>
#include "eventloop.h"
#include "epoller.h"
#include "channel.h"
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
    channel.SetReadCallBack(timeout);
    channel.EnableReading();

    itimerspec howlong;
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timefd, 0, &howlong, NULL);
    g_loop->loop();

    return 0;
}