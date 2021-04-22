#include "event/eventloop.h"

#include <thread>
#include <stdio.h>

using namespace std;

void threadFunc()
{
    printf("threadFunc(): pid = %d, tid = %d\n",
           getpid(), this_thread::get_id());

    EventLoop loop;
    loop.loop();
}

int main()
{
    printf("main(): pid = %d, tid = %d\n",
           getpid(), this_thread::get_id());

    EventLoop loop;

    thread t(threadFunc);

    loop.loop();
    t.join();
}