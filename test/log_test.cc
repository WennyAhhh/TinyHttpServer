#include <iostream>
#include <unistd.h>
#include "../src/log.h"

int main()
{
    std::cout << "xx" << std::endl;
    Log::Instance()->init("./log", true, true);
    sleep(1);
    while (1)
    {
        sleep(0.1);
        LOG_DEBUG("test for LOG,args: number:[%d], string:[%s]", 1, "yoyoyo");
    }
    // LOG("test for LOG,args: number:[%d], string:[%s]", 1, "yoyoyo");
    // sleep(2);
    return 0;
}