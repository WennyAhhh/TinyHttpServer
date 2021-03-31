#include "../src/event/eventloop.h"
#include <iostream>
#include <thread>
#include <unistd.h>

using namespace std;

void threadFunc()
{
    printf("threadFunc(): pid = %d\n", static_cast<int>(getpid()));
    EventLoop loop;
    loop.loop();
}

int main()
{
    Log::Instance()->init("./log", true, true);
    sleep(1);
    printf("threadFunc(): pid = %d \n", static_cast<int>(getpid()));
    EventLoop loop;
    std::thread newthread(threadFunc);
    loop.loop();
    newthread.join();
    return 0;
}