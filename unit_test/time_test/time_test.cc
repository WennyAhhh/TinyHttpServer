#include <iostream>
#include "event/eventloop.h"

using namespace std;

EventLoop *g_loop;

void print(string brand)
{
    cout << "xiao mi print  " << brand << endl;
}

void cancel(TimerId timer)
{
    g_loop->cancel(timer);
    cout << "cancel" << endl;
    g_loop->quit();
}

void func()
{
    // TimerId id1 = g_loop->run_after(2, bind(print, "MIX"));
    // TimerId id2 = g_loop->run_after(3, std::bind(print, "PRO"));
    // TimerId id3 = g_loop->run_after(4, std::bind(print, "PRO1"));
    // TimerId id4 = g_loop->run_after(7, std::bind(print, "PRO2"));
    // TimerId id5 = g_loop->run_after(2.1, std::bind(print, "PRO3"));
    // TimerId id6 = g_loop->run_after(3.9, std::bind(print, "PRO4"));
    // // TimerId id7 = g_loop->run_after(0.1, std::bind(print, "PRO5"));
    // // TimerId id8 = g_loop->run_after(1, std::bind(print, "PRO6"));
    // g_loop->run_after(7, bind(cancel, id1), false);
    // g_loop->run_after(4, bind(cancel, id2), false);
    // g_loop->run_after(5, bind(cancel, id3), false);
    // g_loop->run_after(8, bind(cancel, id4), false);
    // g_loop->run_after(4, bind(cancel, id5), false);
    // g_loop->run_after(4, bind(cancel, id6), false);
    // g_loop->run_after(4, bind(cancel, id7), false);
    // g_loop->run_after(4, bind(cancel, id8), false);
    TimerId id5 = g_loop->run_after(1, std::bind(print, "PRO3"));
    // for (int i = 0; i < 10; i++)
    // {
    //     g_loop->extend_timer(2, id5);
    //     cout << "extend" << endl;
    // }
    g_loop->run_after(3, bind(cancel, id5), false);
}

int main()
{
    sleep(1);
    std::cout << "xx" << std::endl;
    Log::instance()->init("./.log", true, true);
    EventLoop loop;
    g_loop = &loop;
    std::thread tar(bind(func));
    tar.join();
    cout << "finish" << endl;
    loop.loop();
    return 0;
}