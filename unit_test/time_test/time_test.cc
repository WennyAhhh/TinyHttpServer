#include <iostream>
#include <event/eventloop.h>

using namespace std;

EventLoop *g_loop;

void print()
{
    cout << "xiao mi print" << endl;
}

int main()
{
    sleep(2);
    std::cout << "xx" << std::endl;
    Log::instance()->init("./.log", true, true);
    std::cout << "--------------test-------------" << std::endl;
    EventLoop loop;
    g_loop = &loop;
    loop.run_after(10086, 10, std::bind(print));

    loop.loop();

    return 0;
}